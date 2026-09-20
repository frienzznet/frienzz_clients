module.exports = {
  "transpileDependencies": [
    "vuetify"
  ],
  pwa: {
    // ...Other pwa config options
    workboxPluginMode: "InjectManifest",
    workboxOptions: {
      swSrc: "src/frienzz-service-worker.js"
    },
    icons: {
      favicon96: 'img/icons/favicon-96x96.png',
      appleTouchIcon: 'img/icons/apple-touch-icon.png',
      maskIcon: 'img/icons/favicon.svg',
    }
  },
  chainWebpack: (config) => {
    config.module
      .rule("mjs")
      .test(/\.mjs$/)
      .type("javascript/auto")
      .include.add(/node_modules/)
      .end();
  },
}