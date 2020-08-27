const {expect} = require("chai")
  .use(require("chai-bytes"));

const {Random} = require("../lib/index.js");

describe("Random", () => {
  describe("static #switchAlgorithm", () => {
    it("should not return anything", () => {
      const output = Random.switchAlgorithm("system");
      expect(output).to.equal(undefined);
    });
    it("should not throw when called with a valid algorithm string", () => {
      expect(() => Random.switchAlgorithm("system")).to.not.throw();
    });
    it("should throw when called with an invalid algorithm string", () => {
      expect(() => Random.switchAlgorithm("invalid algorithm")).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      expect(() => Random.switchAlgorithm(123)).to.throw();
    });
    it("should throw when called without arguments", () => {
      expect(() => Random.switchAlgorithm()).to.throw();
    });
  });

  describe("static #randomBytes", () => {
    it("should return a Buffer", () => {
      const randomBytes = Random.randomBytes(100);
      expect(randomBytes).to.be.an.instanceof(Buffer);
    });
    // A (1 / 256) ^ 100 chance of incorrectly failing is good enough
    it("should return a non-zero Buffer", () => {
      const randomBytes = Random.randomBytes(100);
      const zeroBuffer = Buffer.alloc(100);
      expect(randomBytes).to.not.equalBytes(zeroBuffer);
    });
    it("should return a Buffer with the correct length", () => {
      const randomBytes100 = Random.randomBytes(100);
      const randomBytes273 = Random.randomBytes(273);
      expect(randomBytes100.length).to.equal(100);
      expect(randomBytes273.length).to.equal(273);
    });
    it("should throw when called with an invalid type", () => {
      expect(() => Random.randomBytes("invalid type")).to.throw();
    });
    it("should throw when called with a negative number of bytes", () => {
      expect(() => Random.randomBytes(-1)).to.throw();
    });
    it("should throw when called without arguments", () => {
      expect(() => Random.randomBytes()).to.throw();
    });
  });

  describe("static #initNistKat", () => {
    it("should not return anything", () => {
      const entropy = Buffer.alloc(48, "entropy");
      const personalizationString = Buffer.alloc(48, "TCosmo");
      const output1 = Random.initNistKat(entropy);
      const output2 = Random.initNistKat(entropy, personalizationString);
      expect(output1).to.equal(undefined);
      expect(output2).to.equal(undefined);
    });
    it("should throw when called with an invalid type", () => {
      const entropy = Buffer.alloc(48, "entropy");
      expect(() => Random.initNistKat("invalid type")).to.throw();
      expect(() => Random.initNistKat(entropy, "invalid type")).to.throw();
    });
    it("should throw when called with an entropy Buffer that is not exactly 48 bytes long", () => {
      const badEntropy1 = Buffer.alloc(47, "entropy");
      const badEntropy2 = Buffer.alloc(49, "entropy");
      const badEntropy3 = Buffer.alloc(0);
      expect(() => Random.initNistKat(badEntropy1)).to.throw();
      expect(() => Random.initNistKat(badEntropy2)).to.throw();
      expect(() => Random.initNistKat(badEntropy3)).to.throw();
    });
    it("should throw when called with a personalization string Buffer less than 48 bytes long", () => {
      const entropy = Buffer.alloc(48, "entropy");
      const badPersonalizationString = Buffer.alloc(47, "TCosmo");
      expect(() => Random.initNistKat(entropy, badPersonalizationString)).to.throw();
    });
    it("should throw when called without arguments", () => {
      expect(() => Random.initNistKat()).to.throw();
    });
  });
});
