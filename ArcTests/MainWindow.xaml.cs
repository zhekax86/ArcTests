using System;
using System.Collections.Generic;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using Microsoft.Win32;
using System.IO;
using CompLibrary;

namespace ArcTests
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private string[] AvailableActions;
        private bool loadingapp=true;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = false;
            ofd.ValidateNames = true;
            if ( ofd.ShowDialog(this) == true)
                FileName.Text = ofd.FileName ;
        }

        private List<ComboBox> GetComboBoxes()
        {
            List<ComboBox> list = new List<ComboBox>();

            foreach (var c in ComboStack.Children)
                if (c is ComboBox)
                    list.Add((ComboBox)c);

            return list;
        }

        private void ChangeOperCount()
        {
            int cur = ComboStack.Children.Count;
            int _new = int.Parse(OperCount.Text);

            if (_new > cur)
            {
                for (int i = cur; i < _new; i++)
                {
                    ComboBox cmb;
                    cmb = new ComboBox();
                    cmb.Margin = new Thickness(0, 5, 0, 0);
                    cmb.Width = Double.NaN;
                    cmb.Height = 24;
                    foreach (string name in AvailableActions)
                    {
                        ComboBoxItem ci = new ComboBoxItem();
                        ci.Content = name;
                        cmb.Items.Add(ci);
                    }
                    cmb.SelectedIndex = 0;
                    ComboStack.Children.Add(cmb);
                }
            }
            else if (_new < cur)
            {
                ComboStack.Children.RemoveRange(_new-1, ComboStack.Children.Count - _new);
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            List<ComboBox> ComboList = GetComboBoxes();
            AvailableActions = Compressor.GetActionList();

            foreach (var cb in ComboList)
            {
                foreach (string name in AvailableActions)
                {
                    ComboBoxItem ci = new ComboBoxItem();
                    ci.Content = name;
                    cb.Items.Add(ci);
                }
                cb.SelectedIndex = 0;
            }

            loadingapp = false;
        }

        private async void GoButton_Click(object sender, RoutedEventArgs e)
        {
            if (!File.Exists(FileName.Text))
            {
                MessageBox.Show(this, "Такой файл не существует!", "Ошибка", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }


            List<ComboBox> list = GetComboBoxes();
            int[] actions = new int[list.Count];
            for (int i = 0; i < list.Count; i++)
                actions[i] = list[i].SelectedIndex;

            Task job;
            bool Dump = DumpBox.IsChecked == true ? true : false;
            string filename = FileName.Text;

            string ex = FileName.Text.Substring(FileName.Text.LastIndexOf('.')+1);
            if (ex == "compr")
            {
                string str = FileName.Text;
                StringBuilder uncomprname = new StringBuilder(str.Substring(0,str.LastIndexOf('\\')+1));
                str = str.Substring(str.LastIndexOf('\\') + 1);
                str = str.Substring(0,str.LastIndexOf('.'));
                uncomprname.Append(str.Substring(0, str.LastIndexOf('.')));
                uncomprname.Append("_orig");
                uncomprname.Append(str.Substring(str.LastIndexOf('.')));

                job = Task.Run( () => Compressor.Decompress(filename, uncomprname.ToString(), actions, Dump) );
            }
            else
                job = Task.Run( () => Compressor.Compress(filename, filename + ".compr", actions, Dump) );

            foreach (ComboBox cb in list)
                cb.IsEnabled = false;
            GoButton.IsEnabled = false;
            DumpBox.IsEnabled = false;
            FileName.IsEnabled = false;
            BrowseButton.IsEnabled = false;
            IncOpButton.IsEnabled = false;
            DecOpButton.IsEnabled = false;
            OperCount.IsEnabled = false;
            Progress.IsIndeterminate = true;

            await job;

            Progress.IsIndeterminate = false;
            foreach (ComboBox cb in list)
                cb.IsEnabled = true;
            GoButton.IsEnabled = true;
            DumpBox.IsEnabled = true;
            FileName.IsEnabled = true;
            BrowseButton.IsEnabled = true;
            IncOpButton.IsEnabled = true;
            DecOpButton.IsEnabled = true;
            OperCount.IsEnabled = true;

        }

        private void DecOpButton_Click(object sender, RoutedEventArgs e)
        {
            int NewCount = int.Parse(OperCount.Text) - 1;
            if (NewCount < 1) return;
            OperCount.Text = NewCount.ToString();
        }

        private void IncOpButton_Click(object sender, RoutedEventArgs e)
        {
            int NewCount = int.Parse(OperCount.Text) + 1;
            OperCount.Text = NewCount.ToString();
        }

        private void OperCount_PreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            e.Handled = "0123456789".IndexOf(e.Text) < 0;
        }

        private void OperCount_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!loadingapp)
            {
                if (OperCount.Text.Length == 0 ||  int.Parse(OperCount.Text) == 0)
                    OperCount.Text = "1";

                ChangeOperCount();
            }
        }
    }
}
