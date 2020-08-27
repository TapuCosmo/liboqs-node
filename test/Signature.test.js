const {expect} = require("chai")
  .use(require("chai-bytes"));

const {
  Signature,
  Sigs
} = require("../lib/index.js");

describe("Signature", () => {
  describe("constructor", () => {
    it("should be constructible", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      expect(() => new Signature(algorithms[0])).to.not.throw();
    });
    it("should accept a secret key", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(signature.exportSecretKey()).to.equalBytes(secretKey);
    });
    it("should throw when called with an invalid algorithm", () => {
      expect(() => new Signature("invalid algorithm")).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      expect(() => new Signature(123)).to.throw();
      expect(() => new Signature(algorithms[0], "invalid secret key")).to.throw();
    });
  });

  describe("#getDetails", () => {
    it("should return an object", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const output = signature.getDetails();
      expect(output).to.be.an.instanceof(Object);
    });
    it("should return the details with the correct types", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const output = signature.getDetails();
      expect(output.name).to.be.a("string");
      expect(output.version).to.be.a("string");
      expect(output.claimedNistLevel).to.be.a("number");
      expect(output.isEUFCMA).to.be.a("boolean");
      expect(output.publicKeyLength).to.be.a("number");
      expect(output.secretKeyLength).to.be.a("number");
      expect(output.maxSignatureLength).to.be.a("number");
    });
    it("should not throw", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(() => signature.getDetails()).to.not.throw();
    });
  });

  describe("#generateKeypair", () => {
    it("should return a Buffer", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(signature.generateKeypair()).to.be.an.instanceof(Buffer);
    });
    // Has a negligible chance of randomly failing
    it("should return a non-zero Buffer", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      const algorithmDetails = signature.getDetails();
      const zeroBuffer = Buffer.alloc(algorithmDetails.publicKeyLength);
      const publicKey = signature.generateKeypair();
      expect(publicKey).to.not.equalBytes(zeroBuffer);
    });
    it("should return a Buffer with the correct length", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      const algorithmDetails = signature.getDetails();
      const publicKey = signature.generateKeypair();
      expect(publicKey.length).to.equal(algorithmDetails.publicKeyLength);
    });
  });

  describe("#exportSecretKey", () => {
    it("should return a Buffer", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(signature.exportSecretKey()).to.be.an.instanceof(Buffer);
    });
    it("should export the secret key", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(signature.exportSecretKey()).to.equalBytes(secretKey);
    });
    it("should not throw", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const signature = new Signature(algorithms[0], secretKey);
      expect(() => signature.exportSecretKey()).to.not.throw();
    });
  });

  describe("#sign", () => {
    it("should return a Buffer", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      signature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const output = signature.sign(message);
      expect(output).to.be.an.instanceof(Buffer);
    });
    // Has a negligible chance of randomly failing
    it("should return a non-zero Buffer", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      signature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const output = signature.sign(message);
      const zeroBuffer = Buffer.alloc(output.length);
      expect(output).to.not.equalBytes(zeroBuffer);
    });
    it("should return a Buffer with the correct length", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const algorithmDetails = signature.getDetails();
      signature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const output = signature.sign(message);
      expect(output.length).to.be.at.most(algorithmDetails.maxSignatureLength);
    });
    it("should throw when called without a secret key having been generated", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const message = Buffer.alloc(48, "TCosmo");
      expect(() => signature.sign(message)).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      signature.generateKeypair();
      expect(() => signature.sign("invalid type")).to.throw();
    });
    it("should throw when called with no arguments", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      signature.generateKeypair();
      expect(() => signature.sign()).to.throw();
    });
  });

  describe("#verify", () => {
    it("should return a boolean", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const publicKey = signature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const sig = signature.sign(message);
      const output = signature.verify(message, sig, publicKey);
      expect(output).to.be.a("boolean");
    });
    it("should verify correctly", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const publicKey = signature.generateKeypair();
      const badSignature = new Signature(algorithms[0]);
      const badPublicKey = badSignature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const sig = signature.sign(message);
      const goodOutput = signature.verify(message, sig, publicKey);
      const badOutput = signature.verify(message, sig, badPublicKey);
      expect(goodOutput).to.be.true;
      expect(badOutput).to.be.false;
    });
    it("should throw when called with invalid types", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      const publicKey = signature.generateKeypair();
      const message = Buffer.alloc(48, "TCosmo");
      const sig = signature.sign(message);
      expect(() => signature.verify("invalid type")).to.throw();
      expect(() => signature.verify(message, "invalid type")).to.throw();
      expect(() => signature.verify(message, sig, "invalid type")).to.throw();
    });
    it("should throw when called with no arguments", () => {
      const algorithms = Sigs.getEnabledAlgorithms();
      const signature = new Signature(algorithms[0]);
      expect(() => signature.verify()).to.throw();
    });
  });

  describe("integration", () => {
    const algorithms = Sigs.getEnabledAlgorithms();
    const alice = new Signature(algorithms[0]);
    const bob = new Signature(algorithms[0]);
    const bobPublicKey = bob.generateKeypair();
    const message = Buffer.alloc(48, "TCosmo");
    const signature = bob.sign(message);
    const valid = alice.verify(message, signature, bobPublicKey);

    it("secret keys should be different", () => {
      expect(alice.exportSecretKey()).to.not.equalBytes(bob.exportSecretKey());
    });

    it("signature should be valid", () => {
      expect(valid).to.be.true;
    });
  });
});
