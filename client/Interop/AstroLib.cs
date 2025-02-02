using System;
using System.Runtime.InteropServices;

namespace AstroClient.Interop;

public partial class AstroLib
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CameraConfigC
    {
        public IntPtr id;
        public IntPtr value;
        public IntPtr choices;
        public int choices_len;
        public bool is_read_only;
    }

    [LibraryImport("astrolib.dll")]
    [UnmanagedCallConv(CallConvs = [typeof(System.Runtime.CompilerServices.CallConvCdecl)])]
    public static partial IntPtr decode_camera_config(byte[] data, UIntPtr len);

    [LibraryImport("astrolib.dll")]
    [UnmanagedCallConv(CallConvs = [typeof(System.Runtime.CompilerServices.CallConvCdecl)])]
    public static partial void free_camera_config(IntPtr ptr);

    public static string PtrToStringUTF8(IntPtr ptr)
    {
        return ptr == IntPtr.Zero ? "" : Marshal.PtrToStringUTF8(ptr) ?? string.Empty;
    }

    public static string[] PtrToStringArray(IntPtr arrayPtr, int length)
    {
        if (arrayPtr == IntPtr.Zero || length == 0) return Array.Empty<string>();

        string[] result = new string[length];
        IntPtr[] ptrArray = new IntPtr[length];
        Marshal.Copy(arrayPtr, ptrArray, 0, length);

        for (int i = 0; i < length; i++)
        {
            result[i] = PtrToStringUTF8(ptrArray[i]);
        }

        return result;
    }
}
