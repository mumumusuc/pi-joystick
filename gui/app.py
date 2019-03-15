from sanic import Sanic
from sanic import response
from renderer import render
import json

app = Sanic(__name__)
app.static('/static', './dist/static')


@app.get('/')
async def index(request):
    return response.html(render('index.html'))

@app.websocket('/update')
async def feed(request, ws):
    while True:
        json_data = await ws.recv()
        data = json.loads(json_data)
        '''
            SWITCH_Y        = 0x01,
            SWITCH_B        = 0x02,
            SWITCH_A        = 0x04,
            SWITCH_X        = 0x08,
            SWITCH_L        = 0x10,
            SWITCH_R        = 0x20,
            SWITCH_ZL       = 0x40,
            SWITCH_ZR       = 0x80,
            SWITCH_SELECT   = 0x100,
            SWITCH_START    = 0x200,
            SWITCH_LCLICK   = 0x400,
            SWITCH_RCLICK   = 0x800,
            SWITCH_HOME     = 0x1000,
            SWITCH_CAPTURE  = 0x2000,
            HAT_Top         = 0x00,
            HAT_Top-Right   = 0x01,
            HAT_Right       = 0x02,
            HAT_BOTTOM-RIGHT = 0x03,
            HAT_Bottom      = 0x04,
            HAT_Bottom-Left = 0x05,
            HAT_Left        = 0x06,
            HAT_Top-Left    = 0x07,
            HAT_DEFAULT     = 0x08,
        '''
        button_low = data['Y']|(data['B']<<1)|(data['A']<<2)|(data['X']<<3)|(data['L']<<4)|(data['R']<<5)|(data['ZL']<<6)|(data['ZR']<<7)
        button_hi = data['SELECT']|(data['START']<<1)|(data['LS']<<2)|(data['RS']<<3)|(data['HOME']<<4)|(data['CAP']<<5)
        hat = 8
        if data['UP']:
            hat = 0
        if data['RIGHT']:
            hat = 2
        if data['DOWN']:
            hat = 4
        if data['LEFT']:
            hat = 6
        ls_x = 0
        ls_y = 0
        rs_x = 0
        rs_y = 0
        input = bytes([button_low,button_hi,hat,ls_x,ls_y,rs_x,rs_y,0])
        print(input)
        with open('/dev/hidg0','wb') as js:
            js.write(input)
            js.flush()

@app.listener('after_server_start')
async def notify_server_started(app, loop):
    pass


@app.listener('before_server_stop')
async def notify_server_stopping(app, loop):
    pass

if __name__ == '__main__':
    app.run(host="0.0.0.0", port=8080)
