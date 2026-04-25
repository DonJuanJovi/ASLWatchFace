module.exports = [
  {
    type: 'heading',
    defaultValue: 'ASL Watch Face Settings',
    size: 2
  },
  {
    type: 'text',
    defaultValue: 'Customize colors and font for the watch face.'
  },
  {
    type: 'section',
    items: [
      {
        type: 'color',
        messageKey: 'SETTING_BG_COLOR',
        defaultValue: '000000',
        label: 'Background Color',
        sunlight: false
      },
      {
        type: 'color',
        messageKey: 'SETTING_TIME_COLOR',
        defaultValue: 'FFFFFF',
        label: 'Time Color',
        sunlight: false
      },
      {
        type: 'color',
        messageKey: 'SETTING_INFO_COLOR',
        defaultValue: 'AAAAAA',
        label: 'Info Color',
        sunlight: false
      },
      {
        type: 'select',
        messageKey: 'SETTING_FONT_CHOICE',
        defaultValue: '0',
        label: 'Font',
        options: [
          { label: 'American Sign Language', value: '0' },
          { label: 'Dissaramas Fingerspelling', value: '1' }
        ]
      }
    ]
  },
  {
    type: 'submit',
    defaultValue: 'Save Settings'
  }
];
