// The Vue build version to load with the `import` command
// (runtime-only or standalone) has been set in webpack.base.conf with an alias.
import Vue from 'vue'
import App from './App'
import * as mdc from 'module/material-components-web/index'

Vue.config.productionTip = false

/* eslint-disable no-new */
new Vue({
  el: '#app',
  components: {App},
  template: '<App/>',

})

const selector = '.mdc-button, .mdc-icon-button, .mdc-fab';
[].map.call(document.querySelectorAll(selector), function (el) {
  mdc.ripple.MDCRipple.attachTo(el)
})
