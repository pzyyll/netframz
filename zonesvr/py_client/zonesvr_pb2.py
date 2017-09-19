# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: zonesvr.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='zonesvr.proto',
  package='',
  syntax='proto3',
  serialized_pb=_b('\n\rzonesvr.proto\"3\n\x06MsgRet\")\n\x07\x45rrCode\x12\x0b\n\x07SUCCESS\x10\x00\x12\x11\n\x04\x46\x41IL\x10\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01\"\xa2\x01\n\x06MsgCmd\"\x97\x01\n\x05MsgId\x12\x0b\n\x07MSGINIT\x10\x00\x12\x0e\n\tLOGIN_REQ\x10\x81 \x12\x0e\n\tLOGIN_RSP\x10\x82 \x12\x11\n\x0cZONE_SYN_REQ\x10\x81@\x12\x11\n\x0cZONE_SYN_RSP\x10\x82@\x12\r\n\x08ZONE_SYN\x10\x83@\x12\r\n\x08\x43HAT_REQ\x10\x81`\x12\r\n\x08\x43HAT_RSP\x10\x82`\x12\x0e\n\tCHAT_STAT\x10\x83`\"a\n\x07Persion\x12\x0c\n\x04name\x18\x01 \x01(\x0c\x12\x1d\n\x05point\x18\x02 \x01(\x0b\x32\x0e.Persion.Point\x12\n\n\x02id\x18\x03 \x01(\r\x1a\x1d\n\x05Point\x12\t\n\x01x\x18\x01 \x01(\x05\x12\t\n\x01y\x18\x02 \x01(\x05\"*\n\x08ZoneStat\x12\x1e\n\x0cpersion_list\x18\x01 \x03(\x0b\x32\x08.Persion\"\x18\n\x08LoginReq\x12\x0c\n\x04name\x18\x01 \x01(\x0c\"F\n\x08LoginRsp\x12\x0b\n\x03ret\x18\x01 \x01(\x05\x12\x0f\n\x07\x65rr_msg\x18\x02 \x01(\x0c\x12\x1c\n\tzone_stat\x18\x03 \x01(\x0b\x32\t.ZoneStat\"\'\n\nZoneSynReq\x12\x19\n\x07persion\x18\x01 \x01(\x0b\x32\x08.Persion\"H\n\nZoneSynRsp\x12\x0b\n\x03ret\x18\x01 \x01(\x05\x12\x0f\n\x07\x65rr_msg\x18\x02 \x01(\x0c\x12\x1c\n\tzone_stat\x18\x03 \x01(\x0b\x32\t.ZoneStat\"\'\n\x07ZoneSyn\x12\x1c\n\tzone_stat\x18\x01 \x01(\x0b\x32\t.ZoneStat\"(\n\x07\x43hatReq\x12\x0c\n\x04name\x18\x01 \x01(\x0c\x12\x0f\n\x07\x63ontent\x18\x02 \x01(\x0c\"\'\n\x07\x43hatRsp\x12\x0b\n\x03ret\x18\x01 \x01(\x05\x12\x0f\n\x07\x65rr_msg\x18\x02 \x01(\x0c\"D\n\x08\x43hatStat\x12\x19\n\x07speaker\x18\x01 \x01(\x0b\x32\x08.Persion\x12\x0f\n\x07\x63ontent\x18\x02 \x01(\x0c\x12\x0c\n\x04time\x18\x03 \x01(\rb\x06proto3')
)



_MSGRET_ERRCODE = _descriptor.EnumDescriptor(
  name='ErrCode',
  full_name='MsgRet.ErrCode',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='SUCCESS', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='FAIL', index=1, number=-1,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=27,
  serialized_end=68,
)
_sym_db.RegisterEnumDescriptor(_MSGRET_ERRCODE)

_MSGCMD_MSGID = _descriptor.EnumDescriptor(
  name='MsgId',
  full_name='MsgCmd.MsgId',
  filename=None,
  file=DESCRIPTOR,
  values=[
    _descriptor.EnumValueDescriptor(
      name='MSGINIT', index=0, number=0,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGIN_REQ', index=1, number=4097,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='LOGIN_RSP', index=2, number=4098,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ZONE_SYN_REQ', index=3, number=8193,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ZONE_SYN_RSP', index=4, number=8194,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='ZONE_SYN', index=5, number=8195,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CHAT_REQ', index=6, number=12289,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CHAT_RSP', index=7, number=12290,
      options=None,
      type=None),
    _descriptor.EnumValueDescriptor(
      name='CHAT_STAT', index=8, number=12291,
      options=None,
      type=None),
  ],
  containing_type=None,
  options=None,
  serialized_start=82,
  serialized_end=233,
)
_sym_db.RegisterEnumDescriptor(_MSGCMD_MSGID)


_MSGRET = _descriptor.Descriptor(
  name='MsgRet',
  full_name='MsgRet',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _MSGRET_ERRCODE,
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=17,
  serialized_end=68,
)


_MSGCMD = _descriptor.Descriptor(
  name='MsgCmd',
  full_name='MsgCmd',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
    _MSGCMD_MSGID,
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=71,
  serialized_end=233,
)


_PERSION_POINT = _descriptor.Descriptor(
  name='Point',
  full_name='Persion.Point',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='x', full_name='Persion.Point.x', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='y', full_name='Persion.Point.y', index=1,
      number=2, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=303,
  serialized_end=332,
)

_PERSION = _descriptor.Descriptor(
  name='Persion',
  full_name='Persion',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='Persion.name', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='point', full_name='Persion.point', index=1,
      number=2, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='id', full_name='Persion.id', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_PERSION_POINT, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=235,
  serialized_end=332,
)


_ZONESTAT = _descriptor.Descriptor(
  name='ZoneStat',
  full_name='ZoneStat',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='persion_list', full_name='ZoneStat.persion_list', index=0,
      number=1, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=334,
  serialized_end=376,
)


_LOGINREQ = _descriptor.Descriptor(
  name='LoginReq',
  full_name='LoginReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='LoginReq.name', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=378,
  serialized_end=402,
)


_LOGINRSP = _descriptor.Descriptor(
  name='LoginRsp',
  full_name='LoginRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ret', full_name='LoginRsp.ret', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='err_msg', full_name='LoginRsp.err_msg', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='zone_stat', full_name='LoginRsp.zone_stat', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=404,
  serialized_end=474,
)


_ZONESYNREQ = _descriptor.Descriptor(
  name='ZoneSynReq',
  full_name='ZoneSynReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='persion', full_name='ZoneSynReq.persion', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=476,
  serialized_end=515,
)


_ZONESYNRSP = _descriptor.Descriptor(
  name='ZoneSynRsp',
  full_name='ZoneSynRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ret', full_name='ZoneSynRsp.ret', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='err_msg', full_name='ZoneSynRsp.err_msg', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='zone_stat', full_name='ZoneSynRsp.zone_stat', index=2,
      number=3, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=517,
  serialized_end=589,
)


_ZONESYN = _descriptor.Descriptor(
  name='ZoneSyn',
  full_name='ZoneSyn',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='zone_stat', full_name='ZoneSyn.zone_stat', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=591,
  serialized_end=630,
)


_CHATREQ = _descriptor.Descriptor(
  name='ChatReq',
  full_name='ChatReq',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='name', full_name='ChatReq.name', index=0,
      number=1, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='content', full_name='ChatReq.content', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=632,
  serialized_end=672,
)


_CHATRSP = _descriptor.Descriptor(
  name='ChatRsp',
  full_name='ChatRsp',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='ret', full_name='ChatRsp.ret', index=0,
      number=1, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='err_msg', full_name='ChatRsp.err_msg', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=674,
  serialized_end=713,
)


_CHATSTAT = _descriptor.Descriptor(
  name='ChatStat',
  full_name='ChatStat',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='speaker', full_name='ChatStat.speaker', index=0,
      number=1, type=11, cpp_type=10, label=1,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='content', full_name='ChatStat.content', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='time', full_name='ChatStat.time', index=2,
      number=3, type=13, cpp_type=3, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=715,
  serialized_end=783,
)

_MSGRET_ERRCODE.containing_type = _MSGRET
_MSGCMD_MSGID.containing_type = _MSGCMD
_PERSION_POINT.containing_type = _PERSION
_PERSION.fields_by_name['point'].message_type = _PERSION_POINT
_ZONESTAT.fields_by_name['persion_list'].message_type = _PERSION
_LOGINRSP.fields_by_name['zone_stat'].message_type = _ZONESTAT
_ZONESYNREQ.fields_by_name['persion'].message_type = _PERSION
_ZONESYNRSP.fields_by_name['zone_stat'].message_type = _ZONESTAT
_ZONESYN.fields_by_name['zone_stat'].message_type = _ZONESTAT
_CHATSTAT.fields_by_name['speaker'].message_type = _PERSION
DESCRIPTOR.message_types_by_name['MsgRet'] = _MSGRET
DESCRIPTOR.message_types_by_name['MsgCmd'] = _MSGCMD
DESCRIPTOR.message_types_by_name['Persion'] = _PERSION
DESCRIPTOR.message_types_by_name['ZoneStat'] = _ZONESTAT
DESCRIPTOR.message_types_by_name['LoginReq'] = _LOGINREQ
DESCRIPTOR.message_types_by_name['LoginRsp'] = _LOGINRSP
DESCRIPTOR.message_types_by_name['ZoneSynReq'] = _ZONESYNREQ
DESCRIPTOR.message_types_by_name['ZoneSynRsp'] = _ZONESYNRSP
DESCRIPTOR.message_types_by_name['ZoneSyn'] = _ZONESYN
DESCRIPTOR.message_types_by_name['ChatReq'] = _CHATREQ
DESCRIPTOR.message_types_by_name['ChatRsp'] = _CHATRSP
DESCRIPTOR.message_types_by_name['ChatStat'] = _CHATSTAT
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

MsgRet = _reflection.GeneratedProtocolMessageType('MsgRet', (_message.Message,), dict(
  DESCRIPTOR = _MSGRET,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:MsgRet)
  ))
_sym_db.RegisterMessage(MsgRet)

MsgCmd = _reflection.GeneratedProtocolMessageType('MsgCmd', (_message.Message,), dict(
  DESCRIPTOR = _MSGCMD,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:MsgCmd)
  ))
_sym_db.RegisterMessage(MsgCmd)

Persion = _reflection.GeneratedProtocolMessageType('Persion', (_message.Message,), dict(

  Point = _reflection.GeneratedProtocolMessageType('Point', (_message.Message,), dict(
    DESCRIPTOR = _PERSION_POINT,
    __module__ = 'zonesvr_pb2'
    # @@protoc_insertion_point(class_scope:Persion.Point)
    ))
  ,
  DESCRIPTOR = _PERSION,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:Persion)
  ))
_sym_db.RegisterMessage(Persion)
_sym_db.RegisterMessage(Persion.Point)

ZoneStat = _reflection.GeneratedProtocolMessageType('ZoneStat', (_message.Message,), dict(
  DESCRIPTOR = _ZONESTAT,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ZoneStat)
  ))
_sym_db.RegisterMessage(ZoneStat)

LoginReq = _reflection.GeneratedProtocolMessageType('LoginReq', (_message.Message,), dict(
  DESCRIPTOR = _LOGINREQ,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:LoginReq)
  ))
_sym_db.RegisterMessage(LoginReq)

LoginRsp = _reflection.GeneratedProtocolMessageType('LoginRsp', (_message.Message,), dict(
  DESCRIPTOR = _LOGINRSP,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:LoginRsp)
  ))
_sym_db.RegisterMessage(LoginRsp)

ZoneSynReq = _reflection.GeneratedProtocolMessageType('ZoneSynReq', (_message.Message,), dict(
  DESCRIPTOR = _ZONESYNREQ,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ZoneSynReq)
  ))
_sym_db.RegisterMessage(ZoneSynReq)

ZoneSynRsp = _reflection.GeneratedProtocolMessageType('ZoneSynRsp', (_message.Message,), dict(
  DESCRIPTOR = _ZONESYNRSP,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ZoneSynRsp)
  ))
_sym_db.RegisterMessage(ZoneSynRsp)

ZoneSyn = _reflection.GeneratedProtocolMessageType('ZoneSyn', (_message.Message,), dict(
  DESCRIPTOR = _ZONESYN,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ZoneSyn)
  ))
_sym_db.RegisterMessage(ZoneSyn)

ChatReq = _reflection.GeneratedProtocolMessageType('ChatReq', (_message.Message,), dict(
  DESCRIPTOR = _CHATREQ,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ChatReq)
  ))
_sym_db.RegisterMessage(ChatReq)

ChatRsp = _reflection.GeneratedProtocolMessageType('ChatRsp', (_message.Message,), dict(
  DESCRIPTOR = _CHATRSP,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ChatRsp)
  ))
_sym_db.RegisterMessage(ChatRsp)

ChatStat = _reflection.GeneratedProtocolMessageType('ChatStat', (_message.Message,), dict(
  DESCRIPTOR = _CHATSTAT,
  __module__ = 'zonesvr_pb2'
  # @@protoc_insertion_point(class_scope:ChatStat)
  ))
_sym_db.RegisterMessage(ChatStat)


# @@protoc_insertion_point(module_scope)