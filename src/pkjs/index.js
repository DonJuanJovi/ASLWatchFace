var Clay = require('@rebble/clay');
var clayConfig = require('./config');

new Clay(clayConfig);

Pebble.addEventListener('ready', function() {
  console.log('ASLWatchFace JS ready');
});
