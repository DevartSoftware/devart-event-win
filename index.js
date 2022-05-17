'use strict';

const nonWinResponse = () => undefined
const stub = {
	start: nonWinResponse,
	stop: nonWinResponse 
}

const os = require('os')

module.exports = os.platform() === 'win32' ? require('./build/Release/win-event') : stub