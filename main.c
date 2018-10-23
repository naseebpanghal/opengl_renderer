
/* Temporary*/
const static int width =  1920;
const static int height = 1080;
char *device_name = "embedded";
static int thread_count = 0;
int main()
{
  int ret;
  renderer_init(width,height,thread_num);  
  initRenderer(const_cast<char *>(device_name));
  SetRendererName(const_cast<char *>(device_name));
  setInputResolution(width,height);
  while(1)
  {
    /*Update image pointers*/
    m_lPacket->m_packet_data = //pointer to raw data/decoded frame;
    m_lPacket->m_width = width;
    m_lPacket->m_height = height;
    m_lPacket->m_stride = width * height;
    m_lPacket->m_verStride = 1;//height;
    m_lPacket->m_format = RENDERER_NV12;//RENDERER_YUV420;//Use enum
    m_lPacket->m_rotation = 0; 
    m_lPacket->m_image_buf = NULL;
    m_lPacket->m_Y = m_lPacket->m_packet_data;
    m_lPacket->m_U = (unsigned char *)m_lPacket->m_Y + m_lPacket->m_stride;
    m_lPacket->m_V = (unsigned char *)m_lPacket->m_U + (m_lPacket->m_stride >> 2);


    RenderPacket(m_lPacket);
    pollEvents();

    //  free(m_lPacket->m_packet_data);
    free(m_lPacket);
  }

}
