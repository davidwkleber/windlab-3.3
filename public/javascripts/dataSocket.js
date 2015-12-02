			
			
			if ( dataSocket ) {
				console.log('in socket '+dataSocket);
				if ( dataSocket.connected == 'true' ) {
								console.log('in socket '+dataSocket.connected);
				} else {
					dataSocket = io.connect('http://127.0.0.1:1337');
				}
			} else {
							console.log('no dataSocket');

				 var dataSocket = io.connect('http://127.0.0.1:1337');
			}

			if ( dataSocket ) {
			console.log('dataSocket.connected: '+dataSocket.connected);
			} else {
				console.log('dataSocket does not exist even after io.connect ');
			}
			dataSocket.on('connect', function (connectData) {
					console.log('dataSocket client connected ');
					console.log('dataSocket.onConnect connected: '+dataSocket.connected);
			});
			dataSocket.on('disconnect', function (connectData) {
					console.log('dataSocket client disconnected ');
					console.log('dataSocket.onConnect connected: '+dataSocket.connected);
			});
			
			// This was built for a try at processing or averaging data
			// NOT used at this time.
			var measurementData_limit = 50000;
			var measurementData = [];
			
			function handleSocketData(dataIn) {
				measurementData.push(dataIn);
				if ( measurementData.length > measurementData_limit ) {
					measurementData.shift();
				}
				// console.log("handleSocketData "+ measurementData);
			
			};
			
			// dataSocket.on('updateData', handleSocketData );
