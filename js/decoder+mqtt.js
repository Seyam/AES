var CryptoJS = require("crypto-js");
const mqtt = require('mqtt');


var options = {
    port: 1883,
    host: 'mqtt://broker.datasoft-bd.com',
    clientId: 'mqttjs_' + Math.random().toString(16).substr(2, 8),
    username: 'iotdatasoft',
    password: 'brokeriot2o2o',
    keepalive: 60,
    reconnectPeriod: 1000,
    protocolId: 'MQIsdp',
    protocolVersion: 3,
    clean: true,
    encoding: 'utf8'
};

const client = mqtt.connect('mqtt://broker.datasoft-bd.com', options);



client.on('connect', function () {
    client.subscribe('aes/test'); //REMEMBER TO SUBSCRIBE FIRST TO THE TOPIC TO GET MESSAGE FROM THE BROKER
    // broker.publish('seyam/msg','hi');
    //broker.subscribe('light-status');
    //broker.publish('light-status', state);
    console.log('Connected and subscribed!!!');
});



client.on('message', function (topic, message) {
  if(topic=="aes/test"){
    var msg = JSON.parse(message);  // payload is a JSON value containing information about the published data
    // console.log(msg.data);
    // console.log(msg.iv);
    decrypt(msg.data, msg.iv) 
  }
});



function decrypt(esp8266_data, esp8266_iv) {
	

	// The AES encryption/decryption key to be used.
	var AESKey = '2B7E151628AED2A6ABF7158809CF4F3C';

	var plain_iv =  new Buffer( esp8266_iv , 'base64').toString('hex');
	var iv = CryptoJS.enc.Hex.parse( plain_iv );
	var key= CryptoJS.enc.Hex.parse( AESKey );

	// console.log("\n\nDecrypting...\n");

	// Decrypt
	var bytes  = CryptoJS.AES.decrypt( esp8266_data, key , { iv: iv} );
	var plaintext = bytes.toString(CryptoJS.enc.Base64);
	var decoded_b64msg =  new Buffer(plaintext , 'base64').toString('ascii');
	var decoded_msg =     new Buffer( decoded_b64msg , 'base64').toString('ascii');

	// console.log("Done!\n");
	console.log(decoded_msg);
	console.log("\n\n");
} 


