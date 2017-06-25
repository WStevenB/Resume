namespace SelfTask {
    public partial class Signature : Form {
     
     	//global variables concerning mouse position used in drawing signature
        private bool isDown = false;
        private int oldX = -1, oldY = -1, newX = -1,  newY = -1;
        
        //these are populated from the controlling form
        public string name = "";
        public double hours = 0;
        public DateTime weekstart;

        public Signature() {
            InitializeComponent();
        }

		//occurs when user presses left button on mouse
		//drawing will only occur when isDown is true
        private void SignaturePanel_MouseDown(object sender, MouseEventArgs e) {
            if (e.Button == System.Windows.Forms.MouseButtons.Left) isDown = true;
        }

		//occurs when user moves mouse
		//will record most recent location of mouse, and call drawing function
        private void SignaturePanel_MouseMove(object sender, MouseEventArgs e) {
            if (isDown) {
                Point point = SignaturePanel.PointToClient(Cursor.Position);
                newX = point.X; newY = point.Y;
                DrawLine();
            }
        }

		//occurs when user lets go of left mouse button
		//resets drawing variables
        private void SignaturePanel_MouseUp(object sender, MouseEventArgs e) {
            if (e.Button == System.Windows.Forms.MouseButtons.Left) {
                isDown = false;
                oldX = -1; oldY = -1;
            }
        }
		
		//draws a line on user's screen between most recent mouse position, and second most recent
		//then updates second most recent position
        public void DrawLine() {
            if (oldX != -1 && oldY != -1) {
                Graphics g = SignaturePanel.CreateGraphics();
                Pen pen = new Pen(Color.Black);
                g.DrawLine(pen, newX, newY, oldX, oldY);
            }
            oldX = newX;
            oldY = newY;
        }


		//takes a screenshot of user's computer at the location where the signature and details about hours/name are
		//then encodes that into .jpg format and loads it into a byte array
		//finally a new record is made in the Signature database table
		//the first three fields are populated from the controlling form (Name, WeekStart, Hours)
		//the last field is the byte array representing the screenshot taken earlier
		
        private void SubmitButton_Click(object sender, EventArgs e) {
            Rectangle screen = RectangleToScreen(this.ClientRectangle); 
            Rectangle rect = new Rectangle(SignaturePanel.Left + screen.Left, InfoLabel.Top + screen.Top - 10, 
           	        SignaturePanel.Width, NameLabel.Bottom - InfoLabel.Top + 20);
            Bitmap bmp = new Bitmap(rect.Width, rect.Height, PixelFormat.Format32bppArgb);
            Graphics g = Graphics.FromImage(bmp);
            g.CopyFromScreen(rect.Left, rect.Top, 0, 0, bmp.Size, CopyPixelOperation.SourceCopy);

            ImageCodecInfo jpgEncoder = GetEncoder(ImageFormat.Jpeg);
            System.Drawing.Imaging.Encoder myEncoder = System.Drawing.Imaging.Encoder.Quality;
            EncoderParameters myEncoderParameters = new EncoderParameters(1);
            EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 30L);
            myEncoderParameters.Param[0] = myEncoderParameter;
            GraphicsUnit gu = GraphicsUnit.Pixel;
            float ratio = bmp.GetBounds(ref gu).Height / bmp.GetBounds(ref gu).Width;
            Bitmap scaledImage = new Bitmap(bmp, 220, (int)(220 * ratio));
            byte[] imgBytes = new byte[1000000];
                    try {
                        MemoryStream tmpStream = new MemoryStream();         
                        scaledImage.Save(tmpStream, GetEncoder(ImageFormat.Jpeg), getParameters());
                        tmpStream.Seek(0, SeekOrigin.Begin);
                        tmpStream.Read(imgBytes, 0, 1000000);
                        tmpStream.Close();
                    }
                    catch (Exception errr) { imgBytes = new byte[10]; }           
            try {
                MySqlConnection MyConn = 
                           new MySqlConnection(ConfigurationManager.ConnectionStrings["myString"].ConnectionString);
                MyConn.Open();

                MySqlCommand MyCommand = MyConn.CreateCommand();
                MyCommand.CommandText = "INSERT INTO Signature (Name, WeekStart, Hours, Picture) 
             	       VALUES(@name, @weekstart, @hours, @picture);";
                insertParameter(MyCommand, "@name", DbType.String, name);
                insertParameter(MyCommand, "@weekstart", DbType.Date, weekstart);
                insertParameter(MyCommand, "@hours", DbType.Double, hours);
                insertParameter(MyCommand, "@picture", DbType.Binary, imgBytes);
                MyCommand.ExecuteNonQuery();  
            }
            catch(Exception er) {
                MessageBox.Show(er.Message);
            }
            this.Hide();
        }
  
  		//used to parameterize sql commands in an effort to prevent injection attacks
        private void insertParameter(MySqlCommand com, string name, DbType type, object val) {
            IDataParameter para = com.CreateParameter();
            para.ParameterName = name;
            para.DbType = type;
            para.Value = val;
            com.Parameters.Add(para);
        }
   
		//finds proper codec for desired image format
        private ImageCodecInfo GetEncoder(ImageFormat format) {
            ImageCodecInfo[] codecs = ImageCodecInfo.GetImageDecoders();
            foreach (ImageCodecInfo codec in codecs)
                if (codec.FormatID == format.Guid)
                    return codec;
            return null;
        }

		//sets compression properties of codec
        private EncoderParameters getParameters() {
            System.Drawing.Imaging.Encoder myEncoder = System.Drawing.Imaging.Encoder.Quality;
            EncoderParameters myEncoderParameters = new EncoderParameters(1);
            EncoderParameter myEncoderParameter = new EncoderParameter(myEncoder, 30L);
            myEncoderParameters.Param[0] = myEncoderParameter;
            return myEncoderParameters;
        }

		//warns user if they are about to sign for less than a full week
        private void Signature_Load(object sender, EventArgs e) {
            if (hours < 40) {
                if (!MessageBox.Show("Warning - you are about the sign off on less than 40 hours this period.", "Confirm", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes) {
					this.Hide();
                }
            }
        }

        


    }
}
