//
// module for setting Pitch Angle of the wind turbine blades
//

// use express web server 
// and register this service with its router
var express = require('express');
var router = express.Router();

// inital parameters
pitchAngleValue = 1;

// middleware specific to this route, logs timestamps
// 	Node express mechanism for piping together routines in the request - response stream
//
router.use(function timeLog(req, res, next){
	console.log('pitchAngle Time: ', Date.now());
	next();
})

//
// define the home page route
// 	This was written as a simple Rest API routine, not really used.
//
router.get('/', function(req, res){
console.log('pitchAngle get');
 	res.redirect('/');
})


//
// Post handler for the PitchAngle
//
//	core of the API for pitch Angle, the post is called from the interface Form
//	passing the pitchAngle value to be used 
//
router.post('/', function(req, res, next){

	// deterine if the movement is forward or backwards, based upon the 
console.log('pitchAngle post');
	var forwardOrBack = 'F';
	var diffAngle = 0;
	var serialValue = 0;
	pitchAngleValue = req.param('pitchAngleValue', null);
	
	//		serialListener.send('interfaceData', { pitchAngleValue: pitchAngleValue });

	
		console.log('pitchAngleValue param in PA.js '+ req.param('pitchAngleValue', null));
		console.log('pitchAngleValue value  in PA.js '+ pitchAngleValue);

	// 
	// set the Arduino API call based upon what the interface desired.
	//
	if (pitchAngleValue == "down" ) {
			forwardOrBack = 'P';
			pitchAngleValue = '';
	} else if ( pitchAngleValue == "up" ) {
			forwardOrBack = 'M';
			pitchAngleValue = '';
	} else if ( pitchAngleValue == "initDone" ) {
			forwardOrBack = '';
			pitchAngleValue = '0';
	} else {
			forwardOrBack = 'A';
			pitchAngleValue *= 10;
	}
	//
	//	and send the code via the serialListener methods
	//
		serialListener.send( { arduinoCmd: 'P'+forwardOrBack, value: pitchAngleValue } );		

	// 
	// always return something from this request. 
	//	basically a nothing return, but had to have something else the request 'hung' until timeout.
	//
	res.send('pitch angle page');
})

// 
// A simple Rest API for setting the value, not used.
//
router.put('/', function(req, res, next){
	var spinnerValue = req.body.value;
	res.seeValue = req.body.value;
	res.redirect('/');
})

//
//	used for a Rest API to display an about page that was never created.
//
router.get('/about', function(req, res){
	res.send('About page');
})

//
// and like all routes,
//	have to add the route to the module.exports so it may be used
//
module.exports = router;

	