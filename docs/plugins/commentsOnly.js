// Modify commentsOnly to preserve line numbers

const commentRegex = /\/\*\*[\s\S]+?\*\//g;
const notNewlineRegex = /[^\n]/g;

exports.handlers = {
  beforeParse(e) {
    const comments = e.source.match(commentRegex);
    if (comments) {
      e.source = e.source
        // Isolate the unparseable C++ code
        .split(commentRegex)
        .reduce((acc, cur, i) => {
          return acc +
          // Get rid of the C++ code, but keep lines
          cur.replace(notNewlineRegex, "") +
          // Add back the comment
          comments[i];
        }, "");
    } else {
      // Nothing useful, just output nothing
      e.source = "";
    }
  }
};
