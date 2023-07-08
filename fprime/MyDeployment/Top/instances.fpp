module MyDeployment {

  # ----------------------------------------------------------------------
  # Defaults
  # ----------------------------------------------------------------------

  module Default {
    constant QUEUE_SIZE = 10
    constant STACK_SIZE = 22 * 1024
  }

  # ----------------------------------------------------------------------
  # Active component instances
  # ----------------------------------------------------------------------

  instance rateGroup1: Svc.ActiveRateGroup base id 0x0200 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 3 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::rateGroup1);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  rateGroup1.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::rateGroup1),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::rateGroup1::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::rateGroup1),
    ConfigObjects::rateGroup1::stack    
  );
  """
}

  instance rateGroup2: Svc.ActiveRateGroup base id 0x0300 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 3 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::rateGroup2);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  rateGroup2.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::rateGroup2),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::rateGroup2::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::rateGroup2),
    ConfigObjects::rateGroup2::stack    
  );
  """
}
instance cmdSeq: Svc.CmdSequencer base id 0x0600 \
  queue size Default.QUEUE_SIZE \
  stack size Default.STACK_SIZE \
  priority 10 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::cmdSeq);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  cmdSeq.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::cmdSeq),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::cmdSeq::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::cmdSeq),
    ConfigObjects::cmdSeq::stack    
  );
  """
}

  instance cmdDisp: Svc.CommandDispatcher base id 0x0500 \
    queue size 20 \
    stack size Default.STACK_SIZE \
    priority 4 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::cmdDisp);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  cmdDisp.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::cmdDisp),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::cmdDisp::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::cmdDisp),
    ConfigObjects::cmdDisp::stack    
  );
  """
}

  instance eventLogger: Svc.ActiveLogger base id 0x0B00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 4 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::eventLogger);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  eventLogger.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::eventLogger),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::eventLogger::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::eventLogger),
    ConfigObjects::eventLogger::stack    
  );
  """
}

  instance tlmSend: Svc.TlmChan base id 0x0C00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 2 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::tlmSend);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  tlmSend.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::tlmSend),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::tlmSend::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::tlmSend),
    ConfigObjects::tlmSend::stack    
  );
  """
}

  instance prmDb: Svc.PrmDb base id 0x0D00 \
    queue size Default.QUEUE_SIZE \
    stack size Default.STACK_SIZE \
    priority 10 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::prmDb);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  prmDb.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::prmDb),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::prmDb::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::prmDb),
    ConfigObjects::prmDb::stack    
  );
  """
}

    instance fileUplink: Svc.FileUplink base id 0x0900 \
    queue size 10 \
    stack size Default.STACK_SIZE \
    priority 10 \
{
  phase Fpp.ToCpp.Phases.configObjects """
  K_THREAD_STACK_DEFINE(stack, StackSizes::fileUplink);
  """

  phase Fpp.ToCpp.Phases.startTasks """
  fileUplink.start(
    static_cast<NATIVE_UINT_TYPE>(Priorities::fileUplink),
    static_cast<NATIVE_UINT_TYPE>(K_THREAD_STACK_SIZEOF(ConfigObjects::fileUplink::stack)),
    Os::Task::TASK_DEFAULT, // Default CPU
    static_cast<NATIVE_UINT_TYPE>(TaskIds::fileUplink),
    ConfigObjects::fileUplink::stack    
  );
  """
}

  # ----------------------------------------------------------------------
  # Queued component instances
  # ----------------------------------------------------------------------

  # instance $health: Svc.Health base id 0x2000 \
  #   queue size 25

  # ----------------------------------------------------------------------
  # Passive component instances
  # ----------------------------------------------------------------------

  @ Communications driver.
  instance comm: Drv.ByteStreamDriverModel base id 0x4000 \
    type "Components::ZephyrUartDriver" \ # type specified to select implementor of ByteStreamDriverModel
    at "../../Components/ZephyrUartDriver/ZephyrUartDriver.hpp" # location of above implementor must also be specified

  instance downlink: Svc.Framer base id 0x4100

  instance rateGroupDriver: Svc.RateGroupDriver base id 0x4600

  instance staticMemory: Svc.StaticMemory base id 0x4700

  instance textLogger: Svc.PassiveTextLogger base id 0x4800

  instance fileUplinkBufferManager: Svc.BufferManager base id 0x4400

  instance uplink: Svc.Deframer base id 0x4900

  instance helloWorld: Components.HelloWorld base id 0x0F00

  instance zephyrTime: Components.ZephyrTime base id 0x4500

  instance zephyrTimer: Components.ZephyrTimer base id 0x1000
  
}
