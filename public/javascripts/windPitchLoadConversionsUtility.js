function reutrnWindSpeed( windSpeedValueIn ) {
		var windSpeedValueText = (windSpeedValueIn*0.1456)-0.5523;
		windSpeedValueText =  +(Math.round(windSpeedValueText +"e+1")+"e-1");
		if ( windSpeedValueText < 0 ) {
			windSpeedValueText = 0;
		}		
		return windSpeedValueText;
}	;	

function returnPitchAngle( pitchAngleIn ) {
	return  (pitchAngleIn-101)/10;
};

function returnDummyLoad( dummyLoadIn ) {
	var dummyLoadValueText = (dummyLoadIn-1)*0.5;
	return dummyLoadValueText;
};
