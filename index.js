var _AM2315 = require('./build/Release/AM2315');

//Wrapper for AM2315 Module
exports.read = function(cb) {

	_AM2315.read(function(err, result) {
    	if(err) cb(err);
		if(result) cb(null, result);
	});

};