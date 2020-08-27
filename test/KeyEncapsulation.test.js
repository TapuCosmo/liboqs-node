const {expect} = require("chai")
  .use(require("chai-bytes"));

const {
  KeyEncapsulation,
  KEMs
} = require("../lib/index.js");

describe("KeyEncapsulation", () => {
  describe("constructor", () => {
    it("should be constructible", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      expect(() => new KeyEncapsulation(algorithms[0])).to.not.throw();
    });
    it("should accept a secret key", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(keyEncapsulation.exportSecretKey()).to.equalBytes(secretKey);
    });
    it("should throw when called with an invalid algorithm", () => {
      expect(() => new KeyEncapsulation("invalid algorithm")).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      expect(() => new KeyEncapsulation(123)).to.throw();
      expect(() => new KeyEncapsulation(algorithms[0], "invalid secret key")).to.throw();
    });
  });

  describe("#getDetails", () => {
    it("should return an object", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const output = keyEncapsulation.getDetails();
      expect(output).to.be.an.instanceof(Object);
    });
    it("should return the details with the correct types", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const output = keyEncapsulation.getDetails();
      expect(output.name).to.be.a("string");
      expect(output.version).to.be.a("string");
      expect(output.claimedNistLevel).to.be.a("number");
      expect(output.isINDCCA).to.be.a("boolean");
      expect(output.publicKeyLength).to.be.a("number");
      expect(output.secretKeyLength).to.be.a("number");
      expect(output.ciphertextLength).to.be.a("number");
      expect(output.sharedSecretLength).to.be.a("number");
    });
    it("should not throw", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(() => keyEncapsulation.getDetails()).to.not.throw();
    });
  });

  describe("#generateKeypair", () => {
    it("should return a Buffer", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(keyEncapsulation.generateKeypair()).to.be.an.instanceof(Buffer);
    });
    // Has a negligible chance of randomly failing
    it("should return a non-zero Buffer", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      const algorithmDetails = keyEncapsulation.getDetails();
      const zeroBuffer = Buffer.alloc(algorithmDetails.publicKeyLength);
      const publicKey = keyEncapsulation.generateKeypair();
      expect(publicKey).to.not.equalBytes(zeroBuffer);
    });
    it("should return a Buffer with the correct length", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      const algorithmDetails = keyEncapsulation.getDetails();
      const publicKey = keyEncapsulation.generateKeypair();
      expect(publicKey.length).to.equal(algorithmDetails.publicKeyLength);
    });
  });

  describe("#exportSecretKey", () => {
    it("should return a Buffer", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(keyEncapsulation.exportSecretKey()).to.be.an.instanceof(Buffer);
    });
    it("should export the secret key", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(keyEncapsulation.exportSecretKey()).to.equalBytes(secretKey);
    });
    it("should not throw", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const secretKey = Buffer.alloc(48, "TCosmo");
      const keyEncapsulation = new KeyEncapsulation(algorithms[0], secretKey);
      expect(() => keyEncapsulation.exportSecretKey()).to.not.throw();
    });
  });

  describe("#encapsulateSecret", () => {
    it("should return an object", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const publicKey = keyEncapsulation.generateKeypair();
      const output = keyEncapsulation.encapsulateSecret(publicKey);
      expect(output).to.be.an.instanceof(Object);
    });
    it("should return an object with ciphertext and shared secret buffers", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const publicKey = keyEncapsulation.generateKeypair();
      const output = keyEncapsulation.encapsulateSecret(publicKey);
      expect(output.ciphertext).to.be.an.instanceof(Buffer);
      expect(output.sharedSecret).to.be.an.instanceof(Buffer);
    });
    // Has a negligible chance of randomly failing
    it("should have non-zero Buffers", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      const publicKey = keyEncapsulation.generateKeypair();
      const output = keyEncapsulation.encapsulateSecret(publicKey);
      const ciphertextZeroBuffer = Buffer.alloc(algorithmDetails.ciphertextLength);
      const sharedSecretZeroBuffer = Buffer.alloc(algorithmDetails.sharedSecretLength);
      expect(output.ciphertext).to.not.equalBytes(ciphertextZeroBuffer);
      expect(output.sharedSecret).to.not.equalBytes(sharedSecretZeroBuffer);
    });
    it("should have Buffers with the correct lengths", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      const publicKey = keyEncapsulation.generateKeypair();
      const output = keyEncapsulation.encapsulateSecret(publicKey);
      expect(output.ciphertext.length).to.equal(algorithmDetails.ciphertextLength);
      expect(output.sharedSecret.length).to.equal(algorithmDetails.sharedSecretLength);
    });
    it("should throw when called with an invalid public key", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      keyEncapsulation.generateKeypair();
      const badPublicKey = Buffer.alloc(algorithmDetails.publicKeyLength + 1, "TCosmo");
      expect(() => keyEncapsulation.encapsulateSecret(badPublicKey)).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      keyEncapsulation.generateKeypair();
      expect(() => keyEncapsulation.encapsulateSecret("invalid type")).to.throw();
    });
    it("should throw when called with no arguments", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      keyEncapsulation.generateKeypair();
      expect(() => keyEncapsulation.encapsulateSecret()).to.throw();
    });
  });

  describe("#decapsulateSecret", () => {
    it("should return a Buffer", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const publicKey = keyEncapsulation.generateKeypair();
      const {ciphertext} = keyEncapsulation.encapsulateSecret(publicKey);
      const output = keyEncapsulation.decapsulateSecret(ciphertext);
      expect(output).to.be.an.instanceof(Buffer);
    });
    // Has a negligible chance of randomly failing
    it("should return a non-zero Buffer", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      const publicKey = keyEncapsulation.generateKeypair();
      const {ciphertext} = keyEncapsulation.encapsulateSecret(publicKey);
      const output = keyEncapsulation.decapsulateSecret(ciphertext);
      const zeroBuffer = Buffer.alloc(algorithmDetails.sharedSecretLength);
      expect(output).to.not.equalBytes(zeroBuffer);
    });
    it("should return a Buffer with the correct length", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      const publicKey = keyEncapsulation.generateKeypair();
      const {ciphertext} = keyEncapsulation.encapsulateSecret(publicKey);
      const output = keyEncapsulation.decapsulateSecret(ciphertext);
      expect(output.length).to.equal(algorithmDetails.sharedSecretLength);
    });
    it("should throw when called without a secret key having been generated", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const otherKeyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const publicKey = otherKeyEncapsulation.generateKeypair();
      const {ciphertext} = keyEncapsulation.encapsulateSecret(publicKey);
      expect(() => keyEncapsulation.decapsulateSecret(ciphertext)).to.throw();
    });
    it("should throw when called with an invalid ciphertext", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      const algorithmDetails = keyEncapsulation.getDetails();
      keyEncapsulation.generateKeypair();
      const badCiphertext = Buffer.alloc(algorithmDetails.ciphertextLength + 1, "TCosmo");
      expect(() => keyEncapsulation.decapsulateSecret(badCiphertext)).to.throw();
    });
    it("should throw when called with an invalid type", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      keyEncapsulation.generateKeypair();
      expect(() => keyEncapsulation.decapsulateSecret("invalid type")).to.throw();
    });
    it("should throw when called with no arguments", () => {
      const algorithms = KEMs.getEnabledAlgorithms();
      const keyEncapsulation = new KeyEncapsulation(algorithms[0]);
      keyEncapsulation.generateKeypair();
      expect(() => keyEncapsulation.decapsulateSecret()).to.throw();
    });
  });

  describe("integration", () => {
    const algorithms = KEMs.getEnabledAlgorithms();
    const alice = new KeyEncapsulation(algorithms[0]);
    const bob = new KeyEncapsulation(algorithms[0]);
    const alicePublicKey = alice.generateKeypair();
    bob.generateKeypair();
    const {
      ciphertext,
      sharedSecret: bobSharedSecret
    } = bob.encapsulateSecret(alicePublicKey);
    const aliceSharedSecret = alice.decapsulateSecret(ciphertext);

    it("secret keys should be different", () => {
      expect(alice.exportSecretKey()).to.not.equalBytes(bob.exportSecretKey());
    });

    it("shared secrets should be the same", () => {
      expect(aliceSharedSecret).to.equalBytes(bobSharedSecret);
    });
  });
});
