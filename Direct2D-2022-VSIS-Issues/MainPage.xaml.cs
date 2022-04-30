
using DirectX_VSIS_Engine;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;
using Windows.Storage.Streams;
using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace Direct2D_2022_Issues
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private DXOutput outputImage;
        private VirtualSurfaceImageSource virtualSIS;

        double inputDPI = 96;
        double outputDPI = 186.18181818181819;
        double dPIScaleRatio;
        double dPIScaleRatioInvert;

        public MainPage()
        {
            this.InitializeComponent();

            dPIScaleRatio = inputDPI / outputDPI;
            dPIScaleRatioInvert = outputDPI / inputDPI;

            InitializeDirectX();
        }

        public void InitializeDirectX()
        {
            DirectXVSISImage.Width = 816;
            DirectXVSISImage.Height = 1056;

            Size imageSize = new Size((int)Math.Round(DirectXVSISImage.Width * dPIScaleRatioInvert), (int)Math.Round(DirectXVSISImage.Height * dPIScaleRatioInvert));
            virtualSIS = new VirtualSurfaceImageSource((int)imageSize.Width, (int)imageSize.Height);
            outputImage = new DXOutput((int)imageSize.Width, (int)imageSize.Height, (float)outputDPI);
            outputImage.SetVSIS(virtualSIS);
            outputImage.InitializeDevices();

            DirectXVSISImage.Source = virtualSIS;

            outputImage.Render(true);
        }

        private void Grid_Tapped(object sender, TappedRoutedEventArgs e)
        {
            var position = e.GetPosition((Grid)sender);
            var rect = new Rect(position.X * dPIScaleRatioInvert, position.Y * dPIScaleRatioInvert, 30,30);
            outputImage.Render(rect);
        }

    }
}
