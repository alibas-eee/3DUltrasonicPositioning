const express = require('express')
const app = express()
const path = require('path')

const server = require('http').createServer(app);
const WebSocket = require('ws');
const wss = new WebSocket.Server({ server:server, port:3030  });

var dist = {x:0,y:0,z:0};

const SerialPort = require('serialport');

var myPort = new SerialPort("COM7");
var Readline = SerialPort.parsers.Readline; // make instance of Readline parser
var parser = new Readline(); // make a new parser to read ASCII lines
myPort.pipe(parser); // pipe the serial stream to the parser

myPort.on('open', showPortOpen);
parser.on('data', readSerialData);
myPort.on('close', showPortClose);
myPort.on('error', showError);

function showPortOpen() {
  console.log('port open. Data rate: ' + myPort.baudRate);
}
 
function readSerialData(data) {
  dist = data;
  console.log(dist);
}
 
function showPortClose() {
  console.log('port closed.');
}
 
function showError(error) {
  console.log('Serial port error: ' + error);
}


app.use(express.static(__dirname + '/public'))
app.use('/build/', express.static(path.join(__dirname, 'node_modules/three/build')));
app.use('/jsm/', express.static(path.join(__dirname, 'node_modules/three/examples/jsm')));



wss.on('connection', function connection(ws) {
  console.log('A new client Connected!');
  ws.send('Welcome New Client!');   

  ws.on('message', function incoming(message) {
    console.log('received: %s', message);

     ws.send(dist);
      
  });

});





app.get('/', (req, res) => res.send('Hello World!'))


server.listen(3000, () =>
  console.log('Visit http://127.0.0.1:3000')
);