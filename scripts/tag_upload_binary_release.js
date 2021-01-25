const fs = require("fs");
const path = require("path");

const {Octokit} = require("@octokit/core");

const version = require(path.join(__dirname, "../package.json")).version;
const githubToken = process.env.auth_token;

const owner = "TapuCosmo";
const repo = "liboqs-node";

if (process.env.APPVEYOR_REPO_TAG !== "true") {
  console.log("Not a tagged push; exiting");
  process.exit();
}

if (!githubToken) {
  console.log("No GitHub token; exiting");
  process.exit();
}

const octokit = new Octokit({
  auth: githubToken
});

if (!fs.existsSync(path.join(__dirname, "../build/stage", version))) {
  console.log("No binaries exist; exiting");
  process.exit();
}

const binaries = fs.readdirSync(path.join(__dirname, `../build/stage/${version}`));

if (!binaries.length) {
  console.log("No binaries exist; exiting");
  process.exit();
}

(async () => {
  let res;
  try {
    res = await octokit.request(`GET /repos/{owner}/{repo}/releases/tags/{tag}`, {
      owner,
      repo,
      tag: `v${version}`
    });
  } catch (e) {
    if (e.status === 404) {
      res = await octokit.request("POST /repos/{owner}/{repo}/releases", {
        owner,
        repo,
        tag_name: `v${version}`,
        name: `v${version}`,
        body: `v${version}`,
        prerelease: true
      });
    } else {
      throw e;
    }
  }
  for (const binary of binaries) {
    console.log(`Uploading ${binary}`);
    const binaryPath = path.join(__dirname, "../build/stage", version, binary);
    const stats = fs.statSync(binaryPath);
    await octokit.request(`POST ${res.data.upload_url}`, {
      headers: {
        "Content-Length": stats.size
      },
      name: binary,
      label: binary,
      data: fs.createReadStream(binaryPath)
    });
  }
})();
