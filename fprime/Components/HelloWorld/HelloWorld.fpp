module Components {
    @ Example Component for F Prime FSW framework.
    passive component HelloWorld {

        @ A count of the number of greetings issued
        telemetry GreetingCount: U32

        ##############################################################################
        #### Uncomment the following examples to start customizing your component ####
        ##############################################################################

        # @ Example async command
        # async command COMMAND_NAME(param_name: U32)
        @ LED command
        sync command TOGGLE_LED \
            opcode 0

        # @ Example telemetry counter
        # telemetry ExampleCounter: U64

        @ Example event
        event RateGroupPingRecv(ping_num: U32) severity activity high id 0 format "PingNum {}"

        sync input port schedIn: Svc.Sched

        ###############################################################################
        # Standard AC Ports: Required for Channels, Events, Commands, and Parameters  #
        ###############################################################################

        @ Port for sending telemetry channels to downlink
        telemetry port tlmOut

        @ Command receive port
        command recv port CmdDisp

        @ Command registration port
        command reg port CmdReg

        @ Command response port
        command resp port CmdStatus
        
        @ Event port
        event port Log

        @ Text event port
        text event port LogText

        @ A port for getting the time
        time get port Time

    }
}