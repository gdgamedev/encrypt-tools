# Encrypt Tools
### A native module to node js, with encrypt/decrypt functions to texts and files.

It works with a self-made encryption cipher algorithm.

## ATENTION

As this module is purely self-made, including the algorithm itself, is not a guarantee until now that is secure and isn't recommended to use in real projects. This module is only for tests purposes.

## Installation

```shell
npm install encrypt-tools
```

## How to use it

First, you require it in your javascript:

```js
const ENCT = require("encrypt-tools")
```

A brief description to each function:

### encrypt

Encrypts a string with a key
```js
let text = "Hello World!"
let key = "Secret"

text = ENCT.encrypt(text, key) //Returns something like " ½àíóü³ðØ " without quotes
```

### decrypt

Decrypts a encrypted string (with the same key used to encrypt it)
```js
let text = "Hello World!"
let key = "Secret"

text = ENCT.encrypt(text, key) //Returns something like " ½àíóü³ðØ " without quotes

text = ENCT.decrypt(text, key) //Needs to return the original text, in the case " Hello World! "
```

### encryptFile

Asynchronously encrypt a file with a key to a output path

```js
let inPath = "text.txt"
let key = "Secret"
let outPath = "text2.txt"

ENCT.encryptFile(inPath, key, outPath, (processed, total) => {
  console.log("Progress: ", (processed / total).toFixed(1), "%")
}).then(() => {
  console.log("Finished!")
}).catch((err) => {
  console.log("Error status: ", err)
})
```

The err status can return *1* when input file doesn't exists.

### decryptFile

Asynchronously decrypt a encrypted file (with the same key used to encrypt it) to a output path

```js
let inPath = "text.txt"
let key = "Secret"
let outPath = "text2.txt"

ENCT.encryptFile(inPath, key, outPath, (processed, total) => {
  console.log("Progress: ", (processed / total).toFixed(1), "%")
}).then(() => {
  console.log("Finished!")
}).catch((err) => {
  console.log("Error status: ", err)
})
```

The err status can return *1* when input file doesn't exists and *2* when key is wrong.

### writeFile

Asynchronously write data to a file, and encrypt it with a key

```js
let outPath = "text.txt"
let data = "Hello World!"
let key = "Secret"

ENCT.writeFile(outPath, data, key,(processed, total) => {
  console.log("Progress: ", (processed / total).toFixed(1), "%")
}).then(() => {
  console.log("Finished!")
}).catch((err) => {
  console.log("Error status: ", err)
})
```

### readFile

Asynchronously read data from a encrypted file (with the same key used to encrypt it)

```js
let inPath = "text.txt"
let key = "Secret"

ENCT.readFile(inPath, key,(processed, total) => {
  console.log("Progress: ", (processed / total).toFixed(1), "%")
}).then((data) => {
  console.log(data)
}).catch((err) => {
  console.log("Error status: ", err)
})
```

The err status can return *1* when input file doesn't exists and 2 when key is wrong.
