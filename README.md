Usage
=====

    key = TEA::Key.new(1234, 4567, 2345, 3456)
    data = "bobafett".unpack("N*") # (needs to be a multiple of 4 bytes long)

    TEA.encrypt!(data, key) # encrypts your integer array in place; returns it for convenience

    TEA.decrypt!(data, key) # in place too
    data.pack("N*") # => "bobafett"
    

