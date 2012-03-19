using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using DemoDev;

namespace WpfApplication2
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Trackball _trackball;

        public MainWindow()
        {
            InitializeComponent();

            _trackball = new Trackball();
            _trackball.Attach( this );
            _trackball.Slaves.Add( myViewport3D );
            _trackball.Enabled = true;
        }

        protected override void OnKeyDown( KeyEventArgs e )
        {
            base.OnKeyDown( e );
            if ( e.Key == Key.Escape )
                this.Close();
        }
    }
}
