/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId])
/******/ 			return installedModules[moduleId].exports;
/******/
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			exports: {},
/******/ 			id: moduleId,
/******/ 			loaded: false
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.loaded = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(0);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports, __webpack_require__) {

	__webpack_require__(1);
	module.exports = __webpack_require__(2);


/***/ }),
/* 1 */
/***/ (function(module, exports) {

	/**
	 * Copyright 2024 Google LLC
	 *
	 * Licensed under the Apache License, Version 2.0 (the "License");
	 * you may not use this file except in compliance with the License.
	 * You may obtain a copy of the License at
	 *
	 *     http://www.apache.org/licenses/LICENSE-2.0
	 *
	 * Unless required by applicable law or agreed to in writing, software
	 * distributed under the License is distributed on an "AS IS" BASIS,
	 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	 * See the License for the specific language governing permissions and
	 * limitations under the License.
	 */
	
	(function(p) {
	  if (!p === undefined) {
	    console.error('Pebble object not found!?');
	    return;
	  }
	
	  // Aliases:
	  p.on = p.addEventListener;
	  p.off = p.removeEventListener;
	
	  // For Android (WebView-based) pkjs, print stacktrace for uncaught errors:
	  if (typeof window !== 'undefined' && window.addEventListener) {
	    window.addEventListener('error', function(event) {
	      if (event.error && event.error.stack) {
	        console.error('' + event.error + '\n' + event.error.stack);
	      }
	    });
	  }
	
	})(Pebble);


/***/ }),
/* 2 */
/***/ (function(module, exports) {

	Pebble.addEventListener('ready', function(e) {
	  console.log('TestFace JS ready');
	});
	
	Pebble.addEventListener('showConfiguration', function() {
	  var settings = {};
	  try {
	    settings = JSON.parse(localStorage.getItem('testfaceSettings') || '{}');
	  } catch(e) {}
	
	  var bgColor   = settings.SETTING_BG_COLOR   || '#000000';
	  var timeColor = settings.SETTING_TIME_COLOR || '#ffffff';
	  var infoColor = settings.SETTING_INFO_COLOR || '#aaaaaa';
	
	  var html = [
	    '<!DOCTYPE html>',
	    '<html><head>',
	    '<meta name="viewport" content="width=device-width,initial-scale=1">',
	    '<title>TestFace Settings</title>',
	    '<style>',
	    'body{font-family:-apple-system,sans-serif;margin:0;padding:20px;background:#f0f0f0;}',
	    'h2{color:#333;margin-bottom:20px;}',
	    '.setting{background:#fff;padding:15px;margin:10px 0;border-radius:8px;}',
	    '.setting label{display:block;font-weight:bold;margin-bottom:8px;color:#555;}',
	    'input[type=color]{width:100%;height:44px;border:1px solid #ddd;border-radius:4px;cursor:pointer;padding:2px;}',
	    'button{display:block;width:100%;padding:16px;margin-top:20px;background:#4CAF50;color:#fff;border:none;border-radius:8px;font-size:18px;cursor:pointer;}',
	    '</style>',
	    '</head><body>',
	    '<h2>TestFace Settings</h2>',
	    '<div class="setting">',
	    '<label>Background Color</label>',
	    '<input type="color" id="bg" value="' + bgColor + '">',
	    '</div>',
	    '<div class="setting">',
	    '<label>Time Color</label>',
	    '<input type="color" id="time" value="' + timeColor + '">',
	    '</div>',
	    '<div class="setting">',
	    '<label>Info Color</label>',
	    '<input type="color" id="info" value="' + infoColor + '">',
	    '</div>',
	    '<button onclick="save()">Save Settings</button>',
	    '<script>',
	    'function save(){',
	    'var d={',
	    'SETTING_BG_COLOR:document.getElementById("bg").value,',
	    'SETTING_TIME_COLOR:document.getElementById("time").value,',
	    'SETTING_INFO_COLOR:document.getElementById("info").value',
	    '};',
	    'window.location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify(d));',
	    '}',
	    '</script>',
	    '</body></html>'
	  ].join('');
	
	  Pebble.openURL('data:text/html,' + encodeURIComponent(html));
	});
	
	Pebble.addEventListener('webviewclosed', function(e) {
	  if (!e.response || e.response === 'CANCELLED' || e.response === 'null') {
	    return;
	  }
	
	  var config;
	  try {
	    config = JSON.parse(decodeURIComponent(e.response));
	  } catch(err) {
	    try {
	      config = JSON.parse(e.response);
	    } catch(err2) {
	      console.log('Failed to parse config data');
	      return;
	    }
	  }
	
	  localStorage.setItem('testfaceSettings', JSON.stringify(config));
	
	  var dict = {};
	  var colorKeys = ['SETTING_BG_COLOR', 'SETTING_TIME_COLOR', 'SETTING_INFO_COLOR'];
	  for (var i = 0; i < colorKeys.length; i++) {
	    var key = colorKeys[i];
	    if (config[key]) {
	      dict[key] = parseInt(config[key].replace('#', ''), 16);
	    }
	  }
	
	  Pebble.sendAppMessage(dict,
	    function() { console.log('Settings sent successfully'); },
	    function(e) { console.log('Error sending settings: ' + JSON.stringify(e)); }
	  );
	});


/***/ })
/******/ ]);
//# sourceMappingURL=pebble-js-app.js.map