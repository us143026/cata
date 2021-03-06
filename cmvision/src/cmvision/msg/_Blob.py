"""autogenerated by genmsg_py from Blob.msg. Do not edit."""
import roslib.message
import struct


class Blob(roslib.message.Message):
  _md5sum = "56ed098ed179c6b791d85398b11f5ff6"
  _type = "cmvision/Blob"
  _has_header = False #flag to mark the presence of a Header object
  _full_text = """uint32 red
uint32 green
uint32 blue
uint32 area
uint32 x
uint32 y
uint32 left
uint32 right
uint32 top
uint32 bottom

"""
  __slots__ = ['red','green','blue','area','x','y','left','right','top','bottom']
  _slot_types = ['uint32','uint32','uint32','uint32','uint32','uint32','uint32','uint32','uint32','uint32']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.
    
    The available fields are:
       red,green,blue,area,x,y,left,right,top,bottom
    
    @param args: complete set of field values, in .msg order
    @param kwds: use keyword arguments corresponding to message field names
    to set specific fields. 
    """
    if args or kwds:
      super(Blob, self).__init__(*args, **kwds)
      #message fields cannot be None, assign default values for those that are
      if self.red is None:
        self.red = 0
      if self.green is None:
        self.green = 0
      if self.blue is None:
        self.blue = 0
      if self.area is None:
        self.area = 0
      if self.x is None:
        self.x = 0
      if self.y is None:
        self.y = 0
      if self.left is None:
        self.left = 0
      if self.right is None:
        self.right = 0
      if self.top is None:
        self.top = 0
      if self.bottom is None:
        self.bottom = 0
    else:
      self.red = 0
      self.green = 0
      self.blue = 0
      self.area = 0
      self.x = 0
      self.y = 0
      self.left = 0
      self.right = 0
      self.top = 0
      self.bottom = 0

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    @param buff: buffer
    @type  buff: StringIO
    """
    try:
      _x = self
      buff.write(_struct_10I.pack(_x.red, _x.green, _x.blue, _x.area, _x.x, _x.y, _x.left, _x.right, _x.top, _x.bottom))
    except struct.error, se: self._check_types(se)
    except TypeError, te: self._check_types(te)

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    @param str: byte array of serialized message
    @type  str: str
    """
    try:
      end = 0
      _x = self
      start = end
      end += 40
      (_x.red, _x.green, _x.blue, _x.area, _x.x, _x.y, _x.left, _x.right, _x.top, _x.bottom,) = _struct_10I.unpack(str[start:end])
      return self
    except struct.error, e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    @param buff: buffer
    @type  buff: StringIO
    @param numpy: numpy python module
    @type  numpy module
    """
    try:
      _x = self
      buff.write(_struct_10I.pack(_x.red, _x.green, _x.blue, _x.area, _x.x, _x.y, _x.left, _x.right, _x.top, _x.bottom))
    except struct.error, se: self._check_types(se)
    except TypeError, te: self._check_types(te)

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    @param str: byte array of serialized message
    @type  str: str
    @param numpy: numpy python module
    @type  numpy: module
    """
    try:
      end = 0
      _x = self
      start = end
      end += 40
      (_x.red, _x.green, _x.blue, _x.area, _x.x, _x.y, _x.left, _x.right, _x.top, _x.bottom,) = _struct_10I.unpack(str[start:end])
      return self
    except struct.error, e:
      raise roslib.message.DeserializationError(e) #most likely buffer underfill

_struct_I = roslib.message.struct_I
_struct_10I = struct.Struct("<10I")
