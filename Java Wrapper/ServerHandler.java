/* ----------------------------------------------------------------------------
 * This file was automatically generated by SWIG (http://www.swig.org).
 * Version 3.0.2
 *
 * Do not make changes to this file unless you know what you are doing--modify
 * the SWIG interface file instead.
 * ----------------------------------------------------------------------------- */


public class ServerHandler {
  private long swigCPtr;
  protected boolean swigCMemOwn;

  protected ServerHandler(long cPtr, boolean cMemoryOwn) {
    swigCMemOwn = cMemoryOwn;
    swigCPtr = cPtr;
  }

  protected static long getCPtr(ServerHandler obj) {
    return (obj == null) ? 0 : obj.swigCPtr;
  }

  protected void finalize() {
    delete();
  }

  public synchronized void delete() {
    if (swigCPtr != 0) {
      if (swigCMemOwn) {
        swigCMemOwn = false;
        openpadJNI.delete_ServerHandler(swigCPtr);
      }
      swigCPtr = 0;
    }
  }

  public void setServ(Server value) {
    openpadJNI.ServerHandler_serv_set(swigCPtr, this, Server.getCPtr(value), value);
  }

  public Server getServ() {
    long cPtr = openpadJNI.ServerHandler_serv_get(swigCPtr, this);
    return (cPtr == 0) ? null : new Server(cPtr, false);
  }

  public void onStart() {
    openpadJNI.ServerHandler_onStart(swigCPtr, this);
  }

  public boolean canJoin(Client cli, SWIGTYPE_p_std__string why) {
    return openpadJNI.ServerHandler_canJoin(swigCPtr, this, Client.getCPtr(cli), cli, SWIGTYPE_p_std__string.getCPtr(why));
  }

  public String getName() {
    return openpadJNI.ServerHandler_getName(swigCPtr, this);
  }

  public String getDesc() {
    return openpadJNI.ServerHandler_getDesc(swigCPtr, this);
  }

  public String getIconFilePath() {
    return openpadJNI.ServerHandler_getIconFilePath(swigCPtr, this);
  }

  public int getFilledSlots() {
    return openpadJNI.ServerHandler_getFilledSlots(swigCPtr, this);
  }

  public int getOpenSlots() {
    return openpadJNI.ServerHandler_getOpenSlots(swigCPtr, this);
  }

  public void onJoin(Client cli) {
    openpadJNI.ServerHandler_onJoin(swigCPtr, this, Client.getCPtr(cli), cli);
  }

  public PadConfig getDefaultControls() {
    return new PadConfig(openpadJNI.ServerHandler_getDefaultControls(swigCPtr, this), true);
  }

  public void onDisconnect(Client cli) {
    openpadJNI.ServerHandler_onDisconnect(swigCPtr, this, Client.getCPtr(cli), cli);
  }

  public void onPadUpdate(Client cli, PadUpdateObject update) {
    openpadJNI.ServerHandler_onPadUpdate(swigCPtr, this, Client.getCPtr(cli), cli, PadUpdateObject.getCPtr(update), update);
  }

  public ServerHandler() {
    this(openpadJNI.new_ServerHandler(), true);
  }

}
