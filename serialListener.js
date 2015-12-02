
module.exports = serialListener;

// serialListener
//
//	This is the module which handles communication between the Arduino Serial Port
//	Data is written to and read from the COM port via the node package serialport
//	The incomming data is passed onto the web client via web sockets.
//

var portConfig = require('./portConfig.json');

var serialport = require("serialport");
var SerialPort = serialport.SerialPort; // localize object constructor

console.log('ports ' + portConfig.measurement.port);

	DIserialPort = new SerialPort(portConfig.measurement.port, {
		baudrate: portConfig.measurement.baudrate,
		parser: serialport.parsers.readline("EOL"),
	}, function (err) {
		if (err) console.log('Eroror opening measurement  port: ' +  portConfig.measurement.port);
	});


function sleep(time, callback) {
// serialListener.prototype.sleep(time, callback) {
    var stop = new Date().getTime();
    while(new Date().getTime() < stop + time) {
        ;
    }
    callback();
};

/*
var socketServer;
var socketio = require('socket.io');
socketServer = socketio.listen(app, true);
*/
var dev = true;
function serialListener(devFlag) {
	dev = devFlag
	console.log("serialListener.serialListener: devFlag" + devFlag);
}
function setDevFlag(devFlag) {
	dev = devFlag
	console.log("serialListener.serialListener: devFlag" + devFlag);
}
function socketInit() {

	//
	//http://www.barryvandam.com/node-js-communicating-with-arduino/ 
	//copied from the server.js file
	var receivedData = "";
    var sendData = "";
	var delimiter = "\n";
	
 console.log('serialListenerInit called ');

io = require('socket.io').listen(1337);


console.log('serialListener: setup connection now');

io.sockets.on('connection', function(socket){
	console.log('a user connected');
	console.log('connected socket: '+socket.id);


    socket.on('disconnect', function(){
    console.log('user disconnected');
    console.log('socket disconnected' + socket.id+ " " + socket.disconnected);
  });
  

});
};
 		var dataCount = 0;

   DIserialPort.on("open", function () {
		console.log('serialListener.DIserialPort.on Open ' + portConfig.measurement.port);
		socketInit();
	
		// serialport.flush(); // added by Maik, should empty old data in serial buffer !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

      //  sleep(2000, function() {
		// });
		
		// try to give a stop before we start...
		setTimeout(stopDataFlow, 2000);



	});
 
 
 var sendData = '';
 var receivedData = '';
 var chunksIn = 0;
 var PACKAGE_SIZE = 311;
 
 function checkJson(jsonPackage) {
	// console.log("checkJson size: "+jsonPackage.length);
	if( jsonPackage.length === PACKAGE_SIZE )
		return true;
	else {
		console.log("checkJson ERROR: package size Wrong: "+jsonPackage.length);
		return false;
	}
}


 function handleDIserialPortData(data) {
 
	if( dev == true ) {
		dataCount++;
		console.log("handleDIserialPortData data is: "+dataCount);
	}
	
	
	// drop any packate that is not the correct size
	if( checkJson(data) == true ) {

if( io ) {
 console.log("serialListener.handleDIserialPortData data: "+dataCount);
 debugger;

 // Emit the data without any processing at all, no power added, no checking for a complete JSON package.
  io.emit('updateData', data);
 console.log("serialListener.handleDIserialPortData data: "+dataCount);

 // Emit the data after adding the Power 
//		var jsonWithPower = returnMeasurementsWithPower(data);
//		io.emit('updateData', jsonWithPower);

} else {
	console.log("serialListener.handleDIserialPortData no socket for io");
}	
	} else {
		console.log('serialListener: got bad data '+data);
			//serialport.flush(); // added by Maik, should empty old data in serial buffer !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	}
	data = null;
}; 
 
 function returnMeasurementsWithPower( dataIn ) {

			var thisMeasurement = JSON.parse(dataIn);
			/*
						thisMeasurement.rpm = +thisMeasurement.rpm / 1000;
			var powerCalculation = +thisMeasurement.current * +thisMeasurement.voltage / 1000;
			
 			 var sendJSON = "\n{\n\t  \"power\": \""+powerCalculation+'\",';
			// put in the JSON from the serial input next
		
			sendJSON += JSON.stringify(thisMeasurement).substring(3, JSON.stringify(thisMeasurement).length);
			*/
			
			var powerCalculation = +thisMeasurement.current * +thisMeasurement.voltage / 1000;
			
 			 //var sendJSON = "\n{\n\t  \"power\": "+powerCalculation+',';
			 var sendJSON = "\n{\n\t  \"power\": \""+powerCalculation+'\",';
			// put in the JSON from the serial input next
			sendJSON += dataIn.substring(3, dataIn.length);
			
		//	dataJSON = JSON.parse(dataIn);
		//return dataJSON;
		return sendJSON;
}


 // get a message from the parent Node Express processes
 // 
 //		Used by windSpeed, pitchAngle and dummyLoad to send the values for arduino
 //
 //		results in Arduino being sent code and values via serial port
 //
 process.on('message', function(m) {
	console.log('serialListener.on message: '+ m.arduinoCmd+m.value);
			DIserialPort.write(m.arduinoCmd+m.value, function(err, results) {
				console.log('DI_err ' + err);
				console.log('DI_results ' + results);
			});

});


DIserialPort.on('data', handleDIserialPortData) ;


// an attempt to set dtr and rts
// but the serialPort.set command is not known
//	this despite the fact that the code came from a github example...
function setupComPort() {
  console.log('setupComPort');
    //NOTE: you actually de-assert rts and dtr to toggle!
    DIserialPort.set({rts:true, dtr:true}, function(err, something) {
      console.log('Com Port Setup');
        setTimeout(startDataFlow, 5000);
    });
}

//
//	initialize the data flow, 
//		Sending a stop does not really fix anything, but hey good try.
//
//		after sending the stop, it waits and sends a start
function stopDataFlow() {
		  console.log('stopDataFlow');

	
		DIserialPort.write('ST', function(err, results) {
				console.log('return from ST');
				console.log('DI_err ' + err);
				console.log('DI_results ' + results);
			});
	
			setTimeout(flushBuffer, 2000);
		// setTimeout(startDataFlow, 2000);

}

function flushBuffer() {
	// serialport.flush();
	setTimeout(startDataFlow, 2000);
}
//
//	Starrt the data flow 
//	send the Arduino WebShield an AA 
//	
//		first data packet is dumped for good measure.
function startDataFlow() {
		  console.log('startDataFlow');

		DIserialPort.write('AA', function(err, results) {
				console.log('return from AA');
				console.log('DI_err ' + err);
				console.log('DI_results ' + results);
			});
}