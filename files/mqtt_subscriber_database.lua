local success, luasql = pcall(require, "luasql.sqlite3")

local topicName = "*"
local date = "1900-01-01"
local dateFrom = "1900-01-01"
local dateTo = "3000-01-01"

local pathToDB = "/log/mqtt_subscriber_database.db" 

function get_command_line_args()
    local i = 1
    
    while i <= #arg do        
        if arg[i] == "-t" then
            if arg[i + 1] == nil then
                print("Missing value of ", arg[i], " option")
            else
                topicName = arg[i + 1]
            end
        
        elseif arg[i] == "-d" then
            if arg[i + 1] == nil then
                print("Missing value of ", arg[i], " option")
            elseif is_date(arg[i + 1]) then
                date = arg[i + 1]
            end
            
        elseif arg[i] == "-D" then
            if date ~= "1900-01-01" then
                print("Date option is already entered")
            elseif arg[i + 1] == nil or arg[i + 2] == nil then
                print("Missing value(s) of ", arg[i], " option")
            elseif is_date(arg[i + 1]) and is_date(arg[i + 2]) then
                dateFrom = arg[i + 1]
                dateTo = arg[i + 2]
                i = i + 1
            end
        else
            print("Bad option is entered: ", arg[i])
            return
        end

        i = i + 2
    end 
end


function query_topic_and_dates(topicName, dateFrom, dateTo)
    if topicName ~= "*" then
        return 
        "SELECT * FROM topics " ..
        "WHERE " ..
            "topic ='" .. topicName .. "' AND " ..
            "datetime >= '" .. dateFrom .. " 00:00:00' AND " ..
            "datetime <= '" .. dateTo .. " 23:59:59'"
    else
        return 
        "SELECT * FROM topics " ..
        "WHERE " ..
            "datetime >= '" .. dateFrom .. " 00:00:00' AND " ..
            "datetime <= '" .. dateTo .. " 23:59:59'"
    end
end

function is_date(dateStr)
    local format = "%Y-%m-%d"
    local patern = "^%d%d%d%d%-%d%d%-%d%d$"

    if string.match(dateStr, patern) == nil then
        print("Entered string is not in a date format")
        return false
    end
    
    local year, month, day = dateStr:match("(%d+)-(%d+)-(%d+)")
    local timestamp = os.time({ year = year, month = month, day = day })
    
    if timestamp then
        local parsedDate = os.date(format, timestamp)
        if parsedDate == dateStr then
            return true
        end
    end
    
    print("Entered date is incorrect")
    return false
end

function read_database(database)
    local env = assert(luasql.sqlite3())
    local conn = assert(env:connect(database))

    local cursor, err

    if date ~= "1900-01-01" then
        cursor, err = conn:execute(query_topic_and_dates(topicName, date, date))
    elseif dateFrom ~= "1900-01-01" and dateTo ~= "3000-01-01" then
        cursor, err = conn:execute(query_topic_and_dates(topicName, dateFrom, dateTo))
    else
        cursor, err = conn:execute(query_topic_and_dates(topicName, dateFrom, dateTo)) -- using default values
    end

    if not cursor then
        print("Failed to execute the query:", err)
    else
        local row = cursor:fetch({}, "a")
        while row do
            print("Id:", row.id)
            print("Topic:", row.topic)
            print("Data:", row.data)
            print("Datetime:", row.datetime)
            print()
            row = cursor:fetch({}, "a")
        end
        cursor:close()
    end

    conn:close()
    env:close()
end


if not success then
    print("Failed to import luasql.sqlite3")
    os.exit(1)
end

get_command_line_args()
read_database(pathToDB)