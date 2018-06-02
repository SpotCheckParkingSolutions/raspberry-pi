
def detection(path_to_image, camera_output_dimensions)
  path_to_image = path_to_image
  camera_output_dimensions = camera_output_dimensions
  vehicle = {}
  id = 123 
  vehicle["id"] = id

  app(path_to_image, camera_output_dimensions)

  def app(path_to_image, camera_output_dimensions):
    import numpy as np
    import os
    import six.moves.urllib as urllib
    import sys
    import tarfile
    import tensorflow as tf
    import zipfile
    from collections import defaultdict
    from io import StringIO
    from matplotlib import pyplot as plt
    from PIL import Image
    from utils import label_map_util
    from utils import visualization_utils as vis_util

    if tf.__version__ < '1.4.0':
      raise ImportError('Please upgrade your tensorflow installation to v1.4.* or later!')

    ####Define model, will change to our model###
    MODEL_NAME = 'ssd_mobilenet_v1_coco_2017_11_17'
    MODEL_FILE = MODEL_NAME + '.tar.gz'
    DOWNLOAD_BASE = 'http://download.tensorflow.org/models/object_detection/'

    # Path to frozen detection graph. This is the actual model that is used for the object detection.
    PATH_TO_CKPT = MODEL_NAME + '/frozen_inference_graph.pb'

    # List of the strings that is used to add correct label for each box.
    PATH_TO_LABELS = os.path.join('data', 'mscoco_label_map.pbtxt')

    NUM_CLASSES = 90

    opener = urllib.request.URLopener()
    opener.retrieve(DOWNLOAD_BASE + MODEL_FILE, MODEL_FILE)
    tar_file = tarfile.open(MODEL_FILE)
    for file in tar_file.getmembers():
      file_name = os.path.basename(file.name)
      if 'frozen_inference_graph.pb' in file_name:
        tar_file.extract(file, os.getcwd())

    detection_graph = tf.Graph()
    with detection_graph.as_default():
      od_graph_def = tf.GraphDef()
      with tf.gfile.GFile(PATH_TO_CKPT, 'rb') as fid:
        serialized_graph = fid.read()
        od_graph_def.ParseFromString(serialized_graph)
        tf.import_graph_def(od_graph_def, name='')

    label_map = label_map_util.load_labelmap(PATH_TO_LABELS)
    categories = label_map_util.convert_label_map_to_categories(label_map, max_num_classes=NUM_CLASSES, use_display_name=True)
    category_index = label_map_util.create_category_index(categories)

    ###Prepare images to enter model ###
    def load_image_into_numpy_array(image):
      (im_width, im_height) = image.size
      return np.array(image.getdata()).reshape(
          (im_height, im_width, 3)).astype(np.uint8)

    #Image import
    PATH_TO_TEST_IMAGE_DIR = path_to_image
    TEST_IMAGE_PATHS = [ os.path.join(PATH_TO_TEST_IMAGE_DIR, 'image{}.jpg'.format(i))]

    ###Run model on images###
    with detection_graph.as_default():
      with tf.Session(graph=detection_graph) as sess:
        # Definite input and output Tensors for detection_graph
        image_tensor = detection_graph.get_tensor_by_name('image_tensor:0')
        # Each box represents a part of the image where a particular object was detected.
        detection_boxes = detection_graph.get_tensor_by_name('detection_boxes:0')
        # Each score represent how level of confidence for each of the objects.
        # Score is shown on the result image, together with the class label.
        detection_scores = detection_graph.get_tensor_by_name('detection_scores:0')
        detection_classes = detection_graph.get_tensor_by_name('detection_classes:0')
        num_detections = detection_graph.get_tensor_by_name('num_detections:0')
        for image_path in TEST_IMAGE_PATHS:
          image = Image.open(image_path)
          # the array based representation of the image will be used later in order to prepare the
          # result image with boxes and labels on it.
          image_np = load_image_into_numpy_array(image)
          # Expand dimensions since the model expects images to have shape: [1, None, None, 3]
          image_np_expanded = np.expand_dims(image_np, axis=0)
          # Actual detection.
          (boxes, scores, classes, num) = sess.run(
              [detection_boxes, detection_scores, detection_classes, num_detections],
              feed_dict={image_tensor: image_np_expanded})

        #Return vehicles
        for i,b in enumerate(boxes[0]):
        #cars                   #trucks
            if classes[0][i] == 3 or classes[0][i] == 8:
                if scores[0][i] >= 0.5:
                  tf_vehicle = {}
                  tf_vehicle["x1"] = boxes[0][1][1] #x1
                  tf_vehicle["x2"] = boxes[0][1][3] #x2
                  tf_vehicle["y1"] = boxes[0][1][0] #y1
                  tf_vehicle["y2"] = boxes[0][1][2] #y2
                else:
                  pass
            else:
              pass
        converted_vehicle = create_vehicle(tf_vehicle, camera_output_dimensions[0], camera_output_dimensions[1])
        vehicle["location"] = converted_vehicle


                
    def create_vehicle(tf_vehicle, Xpixels, Ypixels):
      converted_vehicle = {}
      converted_vehicle["x1"] = Xpixels * tf_vehicle["x1"]
      converted_vehicle["x2"] = Xpixels * tf_vehicle["x2"]
      converted_vehicle["y1"] = Xpixels * tf_vehicle["y1"]
      converted_vehicle["y2"] = Xpixels * tf_vehicle["y2"]
      return converted_vehicle

  return vehicle