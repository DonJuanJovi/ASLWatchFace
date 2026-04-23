Pebble.addEventListener('ready', function(e) {
  console.log('ASLWatchFace JS ready');
});

Pebble.addEventListener('showConfiguration', function() {
  var settings = {};
  try {
    settings = JSON.parse(localStorage.getItem('aslWatchFaceSettings') || '{}');
  } catch(e) {}

  var bgColor   = settings.SETTING_BG_COLOR   || '#000000';
  var timeColor = settings.SETTING_TIME_COLOR || '#ffffff';
  var infoColor = settings.SETTING_INFO_COLOR || '#aaaaaa';
  var fontChoice = settings.SETTING_FONT_CHOICE || '0';

  var html = [
    '<!DOCTYPE html>',
    '<html><head>',
    '<meta name="viewport" content="width=device-width,initial-scale=1">',
    '<title>ASL Watch Face Settings</title>',
    '<style>',
    'body{font-family:-apple-system,sans-serif;margin:0;padding:20px;background:#f0f0f0;}',
    'h2{color:#333;margin-bottom:20px;}',
    '.setting{background:#fff;padding:15px;margin:10px 0;border-radius:8px;}',
    '.setting label{display:block;font-weight:bold;margin-bottom:8px;color:#555;}',
    'input[type=color]{width:100%;height:44px;border:1px solid #ddd;border-radius:4px;cursor:pointer;padding:2px;}',
    'select{width:100%;height:44px;border:1px solid #ddd;border-radius:4px;font-size:16px;padding:0 8px;}',
    'button{display:block;width:100%;padding:16px;margin-top:20px;background:#4CAF50;color:#fff;border:none;border-radius:8px;font-size:18px;cursor:pointer;}',
    '</style>',
    '</head><body>',
    '<h2>ASL Watch Face Settings</h2>',
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
    '<div class="setting">',
    '<label>Font</label>',
    '<select id="font">',
    '<option value="0"' + (fontChoice === '0' || fontChoice === 0 ? ' selected' : '') + '>American Sign Language</option>',
    '<option value="1"' + (fontChoice === '1' || fontChoice === 1 ? ' selected' : '') + '>Dissaramas Fingerspelling</option>',
    '</select>',
    '</div>',
    '<button onclick="save()">Save Settings</button>',
    '<script>',
    'function save(){',
    'var d={',
    'SETTING_BG_COLOR:document.getElementById("bg").value,',
    'SETTING_TIME_COLOR:document.getElementById("time").value,',
    'SETTING_INFO_COLOR:document.getElementById("info").value,',
    'SETTING_FONT_CHOICE:document.getElementById("font").value',
    '};',
    'var encoded=encodeURIComponent(JSON.stringify(d));',
    'var m=/(?:^|[?&#])return_to=([^&]*)/.exec((window.location.search||"")+"&"+(window.location.hash||"").replace(/^#/,""));',
    'if(m&&m[1]){window.location.href=decodeURIComponent(m[1])+encoded;return;}',
    'window.location.href="pebblejs://close#"+encoded;',
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

  localStorage.setItem('aslWatchFaceSettings', JSON.stringify(config));

  var dict = {};
  var colorKeys = ['SETTING_BG_COLOR', 'SETTING_TIME_COLOR', 'SETTING_INFO_COLOR'];
  for (var i = 0; i < colorKeys.length; i++) {
    var key = colorKeys[i];
    if (config[key]) {
      dict[key] = parseInt(config[key].replace('#', ''), 16);
    }
  }

  // Font choice (non-color setting)
  if (config.SETTING_FONT_CHOICE !== undefined) {
    dict.SETTING_FONT_CHOICE = parseInt(config.SETTING_FONT_CHOICE, 10);
  }

  Pebble.sendAppMessage(dict,
    function() { console.log('Settings sent successfully'); },
    function(e) { console.log('Error sending settings: ' + JSON.stringify(e)); }
  );
});
