/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_layers_EmbedLiteCompositorProcessParent_h
#define mozilla_layers_EmbedLiteCompositorProcessParent_h

#include "mozilla/layers/CompositorBridgeParent.h"
#include "mozilla/layers/CompositorBridgeChild.h"
#include "Layers.h"
#include "EmbedLiteViewThreadParent.h"

namespace mozilla {
namespace embedlite {

class EmbedLiteCompositorProcessParent final : public CompositorBridgeParent
{
  friend class CompositorBridgeParent;

  NS_INLINE_DECL_THREADSAFE_REFCOUNTING_WITH_MAIN_THREAD_DESTRUCTION(EmbedLiteCompositorProcessParent)
public:
  EmbedLiteCompositorProcessParent(Transport* aTransport, ProcessId aOtherProcess, int aSurfaceWidth, int aSurfaceHeight, uint32_t id);

  virtual void ActorDestroy(ActorDestroyReason aWhy) override;

  virtual mozilla::ipc::IPCResult RecvGetFrameUniformity(FrameUniformityData* aOutData) override { return IPC_OK(); }
  // FIXME/bug 774388: work out what shutdown protocol we need.
  virtual mozilla::ipc::IPCResult RecvPause() override { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvResume() override { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvNotifyChildCreated(const uint64_t& child, CompositorOptions* aOptions) override;
  virtual mozilla::ipc::IPCResult RecvAdoptChild(const uint64_t& child) override { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvMakeSnapshot(const SurfaceDescriptor& aInSnapshot,
                                                   const gfx::IntRect& aRect) { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvFlushRendering() override { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvNotifyRegionInvalidated(
          const nsIntRegion& aRegion) { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvStartFrameTimeRecording(
          const int32_t& aBufferSize, uint32_t* aOutStartIndex) override { return IPC_OK(); }
  virtual mozilla::ipc::IPCResult RecvStopFrameTimeRecording(
          const uint32_t& aStartIndex, InfallibleTArray<float>* intervals) override  { return IPC_OK(); }

  /**
   * Tells this CompositorBridgeParent to send a message when the compositor has received the transaction.
   */
  virtual mozilla::ipc::IPCResult RecvRequestNotifyAfterRemotePaint() override;

  virtual PLayerTransactionParent*
    AllocPLayerTransactionParent(const nsTArray<LayersBackend>& aBackendHints,
                                 const uint64_t& aId) override;

  virtual bool DeallocPLayerTransactionParent(PLayerTransactionParent* aLayers) override;
  virtual bool SetTestSampleTime(const uint64_t& aId,
                                 const TimeStamp& aTime) override;
  virtual void LeaveTestMode(const uint64_t& aId) override;
  virtual void ApplyAsyncProperties(LayerTransactionParent* aLayerTree) override;
  virtual void FlushApzRepaints(const uint64_t& aLayersId) override;
  virtual void GetAPZTestData(const uint64_t& aLayersId,
                              APZTestData* aOutData) override;
  virtual void SetConfirmedTargetAPZC(const uint64_t& aLayersId,
                                      const uint64_t& aInputBlockId,
                                      const nsTArray<ScrollableLayerGuid>& aTargets) override;

  virtual AsyncCompositionManager* GetCompositionManager(LayerTransactionParent* aParent) override;

  virtual mozilla::ipc::IPCResult RecvRemotePluginsReady() override { return IPC_OK(); }

  using CompositorBridgeParentBase::DidComposite;
  void DidComposite(TimeStamp& aCompositeStart, TimeStamp& aCompositeEnd);

  /**
   * A new child process has been configured to push transactions
   * directly to us.  Transport is to its thread context.
   */
  static PCompositorBridgeParent*
  Create(Transport* aTransport, ProcessId aOtherProcess, int aSurfaceWidth, int aSurfaceHeight, uint32_t id);

private:
  // Private destructor, to discourage deletion outside of Release():
  virtual ~EmbedLiteCompositorProcessParent();

  void DeferredDestroy();
  void InitializeLayerManager(const nsTArray<LayersBackend>& aBackendHints);

  // There can be many CPCPs, and IPDL-generated code doesn't hold a
  // reference to top-level actors.  So we hold a reference to
  // ourself.  This is released (deferred) in ActorDestroy().
  RefPtr<EmbedLiteCompositorProcessParent> mSelfRef;
  Transport* mTransport;
  // Child side's process Id.
  base::ProcessId mChildProcessId;

  // If true, we should send a RemotePaintIsReady message when the layer transaction
  // is received
  bool mNotifyAfterRemotePaint;

  RefPtr<LayerManagerComposite> mLayerManager;
  RefPtr<Compositor> mCompositor;
  RefPtr<AsyncCompositionManager> mCompositionManager;
};

} // embedlite
} // mozilla

#endif // mozilla_layers_EmbedLiteCompositorProcessParent_h
