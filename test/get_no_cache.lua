local frandom = io.open("/dev/urandom", "rb")
local d = frandom:read(4)
math.randomseed(d:byte(1) + (d:byte(2) * 256) + (d:byte(3) * 65536) + (d:byte(4) * 4294967296))

logins = {"log", "apple", "test", "random", "a", "global", "text", "ex", "lua"}
number =  math.random(0, table.getn(logins)-1)
login = logins[number]
request = function()
    headers = {}
    headers["Content-Type"] = "application/json"
    body = ''
    return wrk.format("GET", "/account?no_cache&login=".. tostring(login), headers, body)
end