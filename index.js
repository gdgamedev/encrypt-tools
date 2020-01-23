const Encrypt = require('bindings')('encrypt');

console.log(Encrypt)

/**
 * 
 * Encrypts a string with a key
 * 
 * @param {string} input
 * @param {string} key 
 * 
 * @returns {string} A encrypted string
 * 
 * @example
 * 	let enc = encrypt('Hello world!', 'Secret')
 * 	let dec = decrypt(enc, 'Secret')
 */
function encrypt(input, key) {
	return Encrypt.encrypt(input, key)
}

/**
 * 
 * Decrypts a encrypted with a key
 * 
 * @param {string} input
 * @param {string} key 
 * 
 * @returns {string} A decrypted string
 * 
 * @example
 * 	let enc = encrypt('Hello world!', 'Secret')
 * 	let dec = decrypt(enc, 'Secret')
 */
function decrypt(input, key) {
	return Encrypt.decrypt(input, key)
}

/**
 * 
 * Asynchronously encrypt a file with a key to a output path
 * 
 * @param {string} inputPath 
 * @param {string} key 
 * @param {string} outputPath 
 * @param {function} [callback = (progress) => {}]
 * @returns {promise}
 * 
 * @example
 * 	encryptFile('hello.txt', 'Secret', 'hello.txt', (progress) => {
 * 		console.log(progress.processed / progress.total)
 * 	}).then(() => {
 * 		console.log("finished!")
 *  }).catch((err) => {
 * 		console.error(err)
 * 	})
 * 
 */
function encryptFile(inputPath, key, outputPath, callback = (progress) => {}) {
	return Encrypt.encryptFile(inputPath, key, outputPath, callback).then(()=>{}).catch((err)=>{})
}

/**
 * 
 * Asynchronously decrypt a file with a key to a output path
 * 
 * @param {string} inputPath 
 * @param {string} key 
 * @param {string} outputPath 
 * @param {function} [callback = (progress) => {}]
 * @returns {promise} 
 * 
 * @example
 * 	decryptFile('hello.txt', 'Secret', 'hello.txt', (progress) => {
 * 		console.log(progress.processed / progress.total)
 * 	}).then(() => {
 * 		console.log("finished!")
 *  }).catch((err) => {
 * 		console.error(err)
 * 	})
 * 
 */
function decryptFile(inputPath, key, outputPath, callback = (progress) => {}) {
	return Encrypt.decryptFile(inputPath, key, outputPath, callback).then(()=>{}).catch((err)=>{})
}

/**
 * 
 * Asynchronously write data to a file, and encrypt it
 * 
 * @param {string} outputPath 
 * @param {string} key 
 * @param {string} data 
 * @param {function} [callback = (progress) => {}]
 * @returns {promise}
 * 
 * @example
 *	write('hello.txt', 'Secret', 'Hello world!', (progress) => {
 * 		console.log(progress.processed / progress.total)
 * 	}).then(() => {
 * 		console.log("finished!")
 *  }).catch((err) => {
 * 		console.error(err)
 * 	})
 */
function writeFile(outputPath, data, key, callback = (progress) => {}) {
	return Encrypt.writeFile(outputPath, data, key, callback).then(()=>{}).catch((err)=>{})
}

/**
 * 
 * Asynchronously read data from a encrypted file
 * 
 * @param {string} inputPath 
 * @param {string} key 
 * @param {function} [callback = (progress) => {}]
 * @returns {promise}
 * 
 * @example
 *	read('hello.txt', 'Secret', (progress) => {
 * 		console.log(progress.processed / progress.total)
 * 	}).then((data) => {
 * 		console.log(data)
 *  }).catch((err) => {
 * 		console.error(err)
 * 	})
 */
function readFile(inputPath, key, callback = (progress) => {}) {
	return Encrypt.readFile(inputPath, key, callback).then(()=>{}).catch((err)=>{})
}

module.exports = {
	encrypt,
	decrypt,
	encryptFile,
	decryptFile,
	writeFile,
	readFile
}