<template>
  <div class="mdc-card card-size">
    <div class="mdc-card__media  card-content">
      <div class="demo-card__primary">
        <h2 class="demo-card__title mdc-typography mdc-typography--body1">Switch Gamepad</h2>
      </div>

      <div id="gamepad_container" @resize="resizeCanvas()" @mousemove="getCanvasPosition($event)"
           @mouseout="clearCanvasPosition()">
        <canvas id="gamepad" :width="width" :height="height" class="gamepad-canvas"></canvas>
      </div>

      <button v-for="(value, key) in buttons" class="mdc-icon-button"
              @mousedown="button_down(key)" @mouseup="button_up(key)">
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
          CONNECT: {icon: 'videogame_asset', value: false},
        },
      }
    },
    mounted () {
      this.resizeCanvas()
      let canvas = document.querySelector('#gamepad')
      let context = canvas.getContext('2d')
      this.bg.src = require('../assets/gamepad.png')
      this.bg.onload = () => {
        context.drawImage(this.bg,
          0, 0, this.bg.width, this.bg.height,
          0, 0, this.width, this.height)
      }
      const $this = this
      window.onresize = () => {
        return (() => {
          window.screenWidth = document.body.clientWidth
          window.screenHeight = document.body.clientHeight
          $this.resizeCanvas()
        })()
      }
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
      update () {
        let data = {}
        for (let key in this.buttons) {
          data[key] = this.buttons[key].value
        }
        console.log(`data = ${JSON.stringify(data)}`)
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
      getCanvasPosition (e) {
        /*
        let canvas = document.querySelector('#gamepad')
        let context = canvas.getContext('2d')
        let rect = canvas.getBoundingClientRect()
        let x = e.clientX - rect.left
        let y = e.clientY - rect.top

        context.fillStyle = 'white'
        context.fillRect(0, 0, this.width, this.height)

        context.fillStyle = '#aaaaaa'
        context.beginPath()
        context.arc(x, y, 5, 0, Math.PI * 2, true)
        context.closePath()
        context.fill()
        context.drawImage(this.bg,
          0, 0, this.bg.width, this.bg.height,
          0, 0, this.width, this.height)
          */
      }
      ,
      clearCanvasPosition () {

      },
      resizeCanvas () {
        let container = document.querySelector('#gamepad_container')
        let rect = container.getBoundingClientRect()
        this.width = rect.width
        this.height = rect.width * this.ratio
        //console.log(`width[${this.width}] , height[${this.height}]`)
      }
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

  .card-content {
    //height: fit-content;
    //background-size: auto;
    //background-image: url(../assets/gamepad.png);
  }

  #gamepad_container {
  }
</style>
