const fs = require("fs");
const path = require("path");

if (!fs.existsSync(path.join(__dirname, "../deps/liboqs")) || !fs.existsSync(path.join(__dirname, "../deps/liboqs-cpp"))) {
  try {
    fs.rmdirSync(path.join(__dirname, "../deps"), {
      recursive: true
    });
  } catch (e) {}
  if (!fs.existsSync(path.join(__dirname, "../gitsource"))) {
    require("child_process").execSync(`git clone --depth 1 --recurse-submodules --branch v${require("../package.json").version} https://github.com/TapuCosmo/liboqs-node.git gitsource`, {
      cwd: path.join(__dirname, ".."),
      stdio: "inherit"
    });
  }
  fs.renameSync(path.join(__dirname, "../gitsource/deps"), path.join(__dirname, "../deps"));
  try {
    fs.rmdirSync(path.join(__dirname, "../gitsource"), {
      recursive: true
    });
  } catch (e) {}
}
