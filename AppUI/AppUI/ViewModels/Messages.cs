using CommunityToolkit.Mvvm.Messaging.Messages;

namespace AppUI.ViewModels;

internal class CloseMessage { }

internal class TransitionMessage { }

internal class MarkerSelectMessage : ValueChangedMessage<int>
{
    public MarkerSelectMessage(int MarkerNum) : base(MarkerNum) { }
}
internal class VolumeInitialize : ValueChangedMessage<int>
{
    public VolumeInitialize(int MarkerNum) : base(MarkerNum) { }
}
internal class ImageUpdate { }
