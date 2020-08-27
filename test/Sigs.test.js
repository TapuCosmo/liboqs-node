const {expect} = require("chai")
  .use(require("chai-bytes"));

const {Sigs} = require("../lib/index.js");

describe("Sigs", () => {
  describe("static #getEnabledAlgorithms", () => {
    it("should return an array", () => {
      const output = Sigs.getEnabledAlgorithms();
      expect(output).to.be.an.instanceof(Array);
    });
    it("should return an array of strings", () => {
      const output = Sigs.getEnabledAlgorithms();
      const allStrings = !output.some(o => typeof o !== "string");
      expect(allStrings).to.be.true;
    });
    it("should not have default algorithms", () => {
      const output = Sigs.getEnabledAlgorithms();
      const defaultAlgorithms = output.some(o => /default/i.test(o));
      expect(defaultAlgorithms).to.be.false;
    });
    it("should not throw", () => {
      expect(() => Sigs.getEnabledAlgorithms()).to.not.throw();
    });
  });

  describe("static #isAlgorithmEnabled", () => {
    it("should return a bool indicating whether the algorithm is enabled", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const output1 = Sigs.isAlgorithmEnabled(algorithms[0]);
      const output2 = Sigs.isAlgorithmEnabled("invalid algorithm");
      expect(output1).to.be.true;
      expect(output2).to.be.false;
    });
    it("should throw when called with an invalid type", () => {
      expect(() => Sigs.isAlgorithmEnabled(123)).to.throw();
    });
    it("should throw when called without arguments", () => {
      expect(() => Sigs.isAlgorithmEnabled()).to.throw();
    });
  });
});
