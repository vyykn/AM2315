{
    "targets": [{
        "target_name"  : "AM2315"
      , "cflags_cc"      : [
            "-fexceptions"
        ]
      , "cflags"       : [
            "-fexceptions"
          , "-Wno-unused-variable"
        ]
      , "sources"      : [
            "src/bcm2835.c"
          , "AM2315.cpp"
        ]
    }]
}