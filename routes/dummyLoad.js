//
// module for setting Wind Speed of the wind fan
//

// use the expres web server
// and register this service with its router
var express = require('express');
var router = express.Router();

// initial values
dummyLoadValue = 1;

// middleware specific to this route, logs timestamps
router.use(function timeLog(req, res, next){
	console.log('dummyLoad Time: ', Date.now());
	next();
})

// define the home page route
router.get('/', function(req, res){
console.log('dummyLoad get');
 	res.redirect('index');
})


//
// Post is the API service which is used by the interface
//
router.post('/', function(req, res, next){

	dummyLoadValue = req.param('dummyLoadValue', null);
	var serialCallValue = Math.floor((dummyLoadValue-1)*5);
	
		// console.log('dummyLoad serialCall: '+serialCallValue);
		// console.log('dummyLoad rendered index: '+dummyLoadValue);
	serialListener.send( { arduinoCmd: 'II', value: serialCallValue } );

	//
	// send a response, else the request hangs until timeout
	// 	this is a dummy response, but one needs to be sent.
	res.send('dummy  load post');  
})

//
// old idea to set the internal values, not used.
//
router.put('/', function(req, res, next){
	var spinnerValue = req.body.value;
	res.seeValue = req.body.value;
	res.redirect('index');
})

//
// old idea to send an about page, not used
//
router.get('/about', function(req, res){
	res.send('dummy  load about');
})

// 
// and as all, routes must be added to the module.exports to be used.
module.exports = router;

	