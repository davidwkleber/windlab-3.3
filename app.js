//
// app is the Node Server
//
//	This code sets up Node Express, as a web server
//	and the WindLab REST api
//	which handles requests from the interface components

// basic Node packages required
var express = require('express');
var path = require('path');
var favicon = require('static-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');
var dev = true;

function sleep(time, callback) {
// serialListener.prototype.sleep(time, callback) {
    var stop = new Date().getTime();
    while(new Date().getTime() < stop + time) {
        ;
    }
    callback();
};
console.log("serialListener dirname: "+__dirname);
// launch the serialListener as a separate process
//		this will handle communication between the Arduino and the interface
// serialListener = require('child_process').fork(__dirname+'/serialListener.js');
// This will launch the serialListener in the same node process
serialListener = require('./serialListener.js');

// serialListener.setDevFlag(dev);

// an early attempt to call the Arduino measurement board. 
//    sleep(2000, function() {
//		});
 // serialListener.send({ arduinoCmd: 'AA', value: '' });

// this sets up the initial interface page
//		using a variable substitution in index.ejs
 dataFrameContent = 'tourFrame';
 // dataFrameContent = 'lineGraph';
// dataFrameContent = 'powerCurveGraph';
iFrameContent = 'infoFrame';

// classic NodeJS mechanism for setting up REST api methods
var routes = require('./routes/index');

// basically saying that these variables are now the JS modules found in the routes directory
var pitchAngle = require('./routes/pitchAngle');
var windSpeed = require('./routes/windSpeed');
var dummyLoad = require('./routes/dummyLoad');
var guages = require('./routes/guages');
var powerCurve = require('./routes/powerCurve');
// var record = require('./routes/record');
var lineGraph = require('./routes/lineGraph');

// instanciate the app as a Node Express web server
var app = express();


// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');
app.set('view cache', false);

app.use(favicon());
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded());
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', routes);
app.use('index', routes);

// associate the views (web pages such as localhost/pitchAngle) 
//	with the routes (variables defined above). 
//	SO, when the URI localhost/pitchAngle is called at the browser, 
//	the code in module routes/pitchAngle will be called
app.use('/pitchAngle', pitchAngle);
app.use('/windSpeed', windSpeed);
app.use('/powerCurve', powerCurve);
app.use('/guages', guages);

app.use('/dummyLoad', dummyLoad);
app.use('/lineGraph', lineGraph);
// app.use('/record', record);

app.get('/powerCurve', powerCurve.get);
app.get('/guages', guages.get);

/*
	From a version which used API for record,
	not used now

app.post('/recordData', record.recordData);
app.post('/stopRecording', record.stopRecording);
app.post('/saveData', record.saveData);
*/

// a switch used to detect setting of environment variable development 
//	as set in .json or windows env
//	this could be used to provide if statements around debug logging, etc.
//	but was not yet used 
if (app.get('env') === 'development') {
 console.log('App in Dev mode');
} else {
	console.log('App in Prod mode');
};

/// catch 404 and forward to error handler
app.use(function(req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});


// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
	dev = true;
	// serialListener.serialListener(dev);

    app.use(function(err, req, res, next) {
        res.status(err.status || 500);
			console.log('app - error message: '+err.message);
			console.log('app - error : '+err);

        res.render('error', {
            message: err.message,
            error: err
        });
    });
} else {
	dev = false;

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
	console.log('error bad boy ');
 //   res.render('error', {
  //      message: err.message,
  //      error: {}
  //  });

});
}


// Node syntax for registering the Rest API as defined above into a place where other code may access
module.exports = app;

