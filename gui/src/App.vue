<template>
  <div id="app" class="center_content">
    <Gamepad/>
  </div>
</template>

<script>
  import Gamepad from './components/Gamepad'

  export default {
    name: 'App',
    components: {
      Gamepad,
    }
  }
</script>

<style rel="stylesheet/scss" lang="scss">

  @import "material";

  .center_content {
    width: 100%;
    height: 100%;
    display: flex;
    display: -webkit-flex;
    flex-direction: column;
    justify-items: center;
    align-items: center;
  }

  #app {
    font-family: 'Avenir', Helvetica, Arial, sans-serif;
    -webkit-font-smoothing: antialiased;
    -moz-osx-font-smoothing: grayscale;
    text-align: center;
    color: #2c3e50;
  }
</style>
