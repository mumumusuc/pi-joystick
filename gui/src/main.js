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
  mounted () {
    const selector = '.mdc-button, .mdc-fab';
    [].map.call(document.querySelectorAll(selector), function (el) {
      mdc.ripple.MDCRipple.attachTo(el)
    })
    const icon_buttons = document.querySelectorAll('.mdc-icon-button')
    for (let icon of icon_buttons) {
      let instance = new mdc.ripple.MDCRipple(icon)
      instance.unbounded = true
    }
  }
})

