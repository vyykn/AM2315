{
  "targets": [
    {
      "target_name": "AM2315",
      "sources": [ "AM2315.cpp" ],
      "libraries": [ "bcm2835" ]
    },
    {
      "target_name": "bcm2835",
      "type": "static_library",
      "sources": [ "src/bcm2835.cc" ]
    }
  ]
}
