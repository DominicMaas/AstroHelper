from flask import Flask
from flask import jsonify
from flask import Response
from flask import request
import gphoto2 as gp
import time
import io

global CAMERA

PORT = 8080
CAMERA = None

app = Flask(__name__)

def ensure_camera():
    global CAMERA

    # The camera already exists
    if CAMERA != None:
        return

    # Setup the context
    context = gp.Context()

    try:
        # Attempt to get the camera
        CAMERA = gp.Camera()
        CAMERA.init(context)
        print("Camera connected!")
    except:
        CAMERA = None
        print('The camera is not connected')


@app.after_request
def after_request(response):  # Handle CORS
    headers = response.headers
    headers['Access-Control-Allow-Origin'] = '*'
    return response


@app.route('/set-config-item/<name>', methods=['POST'])
def set_config_item(name):
    global CAMERA

    # Handle getting the camera
    ensure_camera()
    if CAMERA == None:
        return 'The camera is not connected', 500

    value = request.data.decode('UTF-8')

    # Get the config
    config = CAMERA.get_config()

    # Get the widget
    OK, widget = gp.gp_widget_get_child_by_name(config, name)
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of ' + name, 500

    OK = gp.gp_widget_set_value(widget, value)
    if OK != gp.GP_OK:
        return 'Could not set the value', 500

    OK = gp.gp_camera_set_config(CAMERA, config)
    if OK != gp.GP_OK:
        return 'Could not set the config', 500

    return 'Value updated successfully'


@app.route('/get-config-item/<name>')
def get_config_item(name):
    global CAMERA

    # Handle getting the camera
    ensure_camera()
    if CAMERA == None:
        return 'The camera is not connected', 500

    # Get the config
    config = CAMERA.get_config()

    # Get the widget
    OK, widget = gp.gp_widget_get_child_by_name(config, name)
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of ' + name, 500

    choices = []

    # Some config may not have choices
    try:
        # Get the value and the choices
        w_val = widget.get_value()
        choices_raw = widget.get_choices()

        # Format choices
        for c in choices_raw:
            choices.append(c)
    except:
        pass

    # Return formatted
    return jsonify(
        value=w_val,
        choices=choices
    )


@app.route('/capture-preview')
def app_capture_preview():
    global CAMERA

    # Handle getting the camera
    ensure_camera()
    if CAMERA == None:
        return 'The camera is not connected', 500

    # Get the config
    config = CAMERA.get_config()

    # Get the widgets
    OK, capturetarget_widget = gp.gp_widget_get_child_by_name(config, 'capturetarget')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of capturetarget or the camera is disconnected', 500

    OK, viewfinder_widget = gp.gp_widget_get_child_by_name(config, 'viewfinder')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of viewfinder or the camera is disconnected', 500

    OK, imagequality_widget = gp.gp_widget_get_child_by_name(config, 'imagequality')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of imagequality or the camera is disconnected', 500

    # Set the capture target to 'Internal RAM'
    OK = gp.gp_widget_set_value(capturetarget_widget, 'Internal RAM')
    if OK != gp.GP_OK:
        return 'Could not set the capture target to Internal RAM', 500

    # Set the view finder to
    OK = gp.gp_widget_set_value(viewfinder_widget, 0)
    if OK != gp.GP_OK:
        return 'Could not set the view finder to 0', 500

    # Set the image format to 'JPEG Normal'
    OK = gp.gp_widget_set_value(imagequality_widget, 'JPEG Normal')
    if OK != gp.GP_OK:
        return 'Could not set the image quality to JPEG Normal', 500

    OK = gp.gp_camera_set_config(CAMERA, config)
    if OK != gp.GP_OK:
        return 'Could not update the camera configuration', 500

    # Capture the image (to memory)
    path = CAMERA.capture(gp.GP_CAPTURE_IMAGE)
    print('capture', path.folder + path.name)
    camera_file = CAMERA.file_get(path.folder, path.name, gp.GP_FILE_TYPE_NORMAL)
    file_data = gp.check_result(gp.gp_file_get_data_and_size(camera_file))

    # Process the image data
    data = memoryview(file_data)
    print(type(data), len(data))

    # Return the image data
    return Response(data.tobytes(), mimetype='image/jpeg')


@app.route('/capture-image', methods=['POST'])
def app_capture_image():
    global CAMERA

    # Handle getting the camera
    ensure_camera()
    if CAMERA == None:
        return 'The camera is not connected', 500

    # Get the config
    config = CAMERA.get_config()

    # Get the widgets
    OK, capturetarget_widget = gp.gp_widget_get_child_by_name(config, 'capturetarget')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of capturetarget or the camera is disconnected', 500

    OK, viewfinder_widget = gp.gp_widget_get_child_by_name(config, 'viewfinder')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of viewfinder or the camera is disconnected', 500

    OK, imagequality_widget = gp.gp_widget_get_child_by_name(config, 'imagequality')
    if OK != gp.GP_OK:
        CAMERA = None
        ensure_camera() # This will also not be okay if the camera disconnects
        return 'There are no widgets with the name of imagequality or the camera is disconnected', 500

    # Set the capture target to 'Memory card'
    OK = gp.gp_widget_set_value(capturetarget_widget, 'Memory card')
    if OK != gp.GP_OK:
        return 'Could not set the capture target to Memory card', 500

    # Set the view finder to 0 (in case it it 1 from the capture preview)
    OK = gp.gp_widget_set_value(viewfinder_widget, 0)
    if OK != gp.GP_OK:
        return 'Could not set the view finder to 0', 500

    # Set the image format back to 'NEF (Raw)'
    OK = gp.gp_widget_set_value(imagequality_widget, 'NEF (Raw)')
    if OK != gp.GP_OK:
        return 'Could not set the image quality to NEF (Raw)', 500

    OK = gp.gp_camera_set_config(CAMERA, config)
    if OK != gp.GP_OK:
        return 'Could not update the camera configuration', 500

    # Capture the image (to the memory card)
    CAMERA.capture(gp.GP_CAPTURE_IMAGE)

    return '', 301



# Start the web server
if __name__ == '__main__':
    ensure_camera()
    app.run(host='0.0.0.0', port=PORT, threaded=False)
