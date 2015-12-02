//
// module for setting Wind Speed of the wind fan
//
//	This is a Restful API based upon the URI windSpeed
//
//

// utilize the Node Express web server 
// 	and insert this service into its router 
//
var express = require('express');
var router = express.Router();

// initial values
windSpeedValue = 0;

// middleware specific to this route, logs timestamps
router.use(function timeLog(req, res, next){
	console.log('windSpeed Time: ', Date.now());
	next();
})

// define the home page route
router.get('/', function(req, res){
console.log('windSpeed get');
 	res.redirect('index');
})

//
// the main Restful Service
//
//	handles requests to change the wind speed.
//
router.post('/', function(req, res, next){

	console.log('windSpeed post');
	console.log('windSpeed value in post: ', req.param('windSpeedValue', null));
	
	//
	// get the wind speed from the URI call params
	//
	windSpeedValue = req.param('windSpeedValue', null);
	
	// var serialCallValue = Math.floor(windSpeedValue*0.625);
	var serialCallValue = Math.floor(windSpeedValue);
	console.log(' rounded wind speed: '+serialCallValue);
 
	console.log('windSpeed rendered index: '+windSpeedValue);
	console.log('windSpeed send to serialListener with : '+serialCallValue);
	
if( serialListener.send() ) {
	console.log("serialListner exists");
 // serialListener.send({ arduinoCmd: 'AA', value: '' });

	//
	// use the serial Listener interface to send codes to the Arduino
	//
	serialListener.send( { arduinoCmd: 'WS', value: serialCallValue } );	
	console.log('windSpeed serialCall done: '+serialCallValue);
		console.log("serialListner send worked");

} else {
	console.log("windSpeed serialListener is null" );
}
	//  must return something, here is a response that does nothing
	res.send('wind speed page');
   
})

//
// used for a simple Rest service which would just set the value of the wind speed
//	Never used and likely incorrect as req.body.value stuff is depricated.
//
router.put('/', function(req, res, next){
	var spinnerValue = req.body.value;
	res.seeValue = req.body.value;
	res.redirect('index');
})

//
// used as a simple Rest service to get the page
//	Never used. 
//
router.get('/about', function(req, res){
	res.send('wind speed About page');
})

//
// all routes must be exported to the module.exports for use in the interface.
//
module.exports = router;

	