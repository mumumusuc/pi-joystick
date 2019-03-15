<template>
  <div class="mdc-card card-size">
    <h2 class="mdc-typography mdc-typography--headline6">Switch Gamepad</h2>

    <div class="mdc-card__media mdc-card__media--16-9 card-header card-content"/>

    <div class="card-content">
      <button @click="connect" class="mdc-button mdc-button--unelevated demo-button-shaped">
        <i class="material-icons mdc-button__icon">videogame_asset</i>
        <span class="mdc-button__label">connect</span>
      </button>
    </div>

    <div>
      <hr>
    </div>

    <div class="card-content">
      <button v-for="(value, key) in buttons" class="mdc-icon-button"
              @mousedown="button_down(key)" @mouseup="button_up(key)" @touchstart="button_down(key)" @touchend="button_up(key)">
        <i class="material-icons mdc-icon-button__icon">{{value.icon}}</i>
      </button>
    </div>

  </div>
</template>

<script>
  export default {
    name: 'Gamepad.vue',
    data: () => {
      return {
        wsUri: 'ws://10.1.1.245:8080/update',
        websocket: null,
        bg: new Image(),
        ratio: 457.0 / 659.0,
        width: 0,
        height: 0,
        buttons: {
          LEFT: {icon: 'arrow_left', value: false},
          UP: {icon: 'arrow_drop_up', value: false},
          RIGHT: {icon: 'arrow_right', value: false},
          DOWN: {icon: 'arrow_drop_down', value: false},
          A: {icon: 'A', value: false},
          B: {icon: 'B', value: false},
          X: {icon: 'X', value: false},
          Y: {icon: 'Y', value: false},
          L: {icon: 'L', value: false},
          R: {icon: 'R', value: false},
          ZL: {icon: 'ZL', value: false},
          ZR: {icon: 'ZR', value: false},
          LS: {icon: 'LS', value: false},
          RS: {icon: 'RS', value: false},
          HOME: {icon: 'home', value: false},
          CAP: {icon: 'panorama_fish_eye', value: false},
          START: {icon: 'add', value: false},
          SELECT: {icon: 'remove', value: false},
        },
      }
    },
    mounted () {
      this.websocket = new WebSocket(this.wsUri)
      this.websocket.onopen = (e) => {
        console.log(`socket open`)
      }
      this.websocket.onclose = (e) => {
        console.log(`socket close`)
      }
      this.websocket.onmessage = (e) => {
        console.log(`socket message:${e.data}`)
      }
      this.websocket.onerror = (e) => {
        console.log(`socket error:${e.data}`)
      }
    },
    methods: {
      connect () {
        if (this.websocket.readyState != 1) {
          return
        }
        this.buttons.L.value = true
        this.buttons.R.value = true
        this.update()
        window.setTimeout(function () {
          this.buttons.L.value = false
          this.buttons.R.value = false
          this.update()
        }, 100)
      },
      update () {
        let data = {}
        for (let key in this.buttons) {
          data[key] = this.buttons[key].value
        }
        //console.log(`data = ${JSON.stringify(data)}`)
        if (this.websocket.readyState == 1) {
          this.websocket.send(JSON.stringify(data))
        }
      },
      button_down (id) {
        this.buttons[id].value = true
        this.update()
      },
      button_up (id) {
        this.buttons[id].value = false
        this.update()
      },
    }
  }
</script>

<style rel="stylesheet/scss" lang="scss" scoped>
  @import "~@material/card/mdc-card";
  @import "~@material/fab/mdc-fab";
  @import "~@material/icon-button/mdc-icon-button";

  .card-size {
    max-width: 600px;
    width: 100%;
    margin: 16px;
    display: flex;
    display: -webkit-flex;
    flex-direction: column;
  }

  .button {
    margin: 4px;
  }

  .card-header {
    background-size: contain;
    background-image: url(../assets/gamepad.png);
  }

  .card-content {
    margin: 8px;
  }

  .demo-button-shaped {
    border-radius: 18px;
  }
</style>
