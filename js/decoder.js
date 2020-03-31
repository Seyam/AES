var CryptoJS = require("crypto-js");
// var request = require('request');

var esp8266_data = 'BR2GDtUpYbuoz+YRG+MIufzdkEMjJZG1KsVAudBsGzaDkSqPHO4VYu572mjRIrxV';
var esp8266_iv  = 'xgI0XUlT1WLAEVdoH/NOTQ==';

// The AES encryption/decryption key to be used.
var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';

var plain_iv =  new Buffer( esp8266_iv , 'base64').toString('hex');
var iv = CryptoJS.enc.Hex.parse( plain_iv );
var key= CryptoJS.enc.Hex.parse( AESKey );

console.log("\n\nDecrypting...\n");

// Decrypt
var bytes  = CryptoJS.AES.decrypt( esp8266_data, key , { iv: iv} );
var plaintext = bytes.toString(CryptoJS.enc.Base64);
var decoded_b64msg =  new Buffer(plaintext , 'base64').toString('ascii');
var decoded_msg =     new Buffer( decoded_b64msg , 'base64').toString('ascii');

console.log("Done!\n");
console.log(decoded_msg);
console.log("\n\n");