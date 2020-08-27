const {expect} = require("chai")
  .use(require("chai-bytes"));

const {KEMs} = require("../lib/index.js");

describe("KEMs", () => {
  describe("static #getEnabledAlgorithms", () => {
    it("should return an array", () => {
      const output = KEMs.getEnabledAlgorithms();
      expect(output).to.be.an.instanceof(Array);
    });
    it("should return an array of strings", () => {
      const output = KEMs.getEnabledAlgorithms();
      const allStrings = !output.some(o => typeof o !== "string");
      expect(allStrings).to.be.true;
    });
    it("should not have default algorithms", () => {
      const output = KEMs.getEnabledAlgorithms();
      const defaultAlgorithms = output.some(o => /default/i.test(o));
      expect(defaultAlgorithms).to.be.false;
    });
    it("should not throw", () => {
      expect(() => KEMs.getEnabledAlgorithms()).to.not.throw();
    });
  });

  describe("static #isAlgorithmEnabled", () => {
    it("should return a bool indicating whether the algorithm is enabled", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const output1 = KEMs.isAlgorithmEnabled(algorithms[0]);
      const output2 = KEMs.isAlgorithmEnabled("invalid algorithm");
      expect(output1).to.be.true;
      expect(output2).to.be.false;
    });
    it("should throw when called with an invalid type", () => {
      expect(() => KEMs.isAlgorithmEnabled(123)).to.throw();
    });
    it("should throw when called without arguments", () => {
      expect(() => KEMs.isAlgorithmEnabled()).to.throw();
    });
  });
});
