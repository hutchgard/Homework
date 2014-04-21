using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;
using WASAPI_Audio.Resources;
using WASAPI_Cpp2;



namespace WASAPI_Audio
{
    public partial class MainPage : PhoneApplicationPage
    {

        private WASAPI_Cpp2.WASAPI AudioLoop = new WASAPI_Cpp2.WASAPI();

        // Constructor
        public MainPage()
        {
            InitializeComponent();
            AudioLoop.Init_Capture();
            // Sample code to localize the ApplicationBar
            //BuildLocalizedApplicationBar();
        }

        private void StartAudio_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            AudioLoop.startThread();
            //if (AudioLoop.)
            this.Output.Text = "Thread Started";
        }

        private void StopAudio_Tap(object sender, System.Windows.Input.GestureEventArgs e)
        {
            AudioLoop.stopThread();
            this.Output.Text = "Thread Stopped";
        }
    }
}