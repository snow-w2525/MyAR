## MyAR�̎g����
	- �ݒ�͂��ׂ�camera-config.json�ōs�����ƁB
	�@���̃t�@�C���̓ǂݍ��݂́A�N���������Ȃ̂ŁA�ݒ��ύX���Ĕ��f�������ꍇ�́A��x�A�v���𗎂Ƃ����ƁB

### JSON�t�@�C���̐ݒ�

#### ���ʐݒ�
	- BACKGROUND_TYPE : string
		window�̔w�i�����ɂ��邩�Bcamera��image����I������
	
	- CAMERA_TITLE : string
		window�̃^�C�g���B

	- CAMERA_NO : int
		BACKGROUND_TYPE��camera�̂Ƃ��ɁA�ǂ̃J�������g�p���邩��ԍ��őI������B

	- BACKGROUND_IMAGE_NAME : string
		BACKGROUND_TYPE��image�̂Ƃ��ɁA�ǂ̉摜��w�i�ɑ}�����邩��ݒ肷��B

	- BACKGROUND_WIDTH : int
		window�̕���ݒ肷��B

	- BACKGROUND_HEIGHT : int
		window�̍�����ݒ肷��B

#### AR���\�[�X�ݒ�
	- COMBINE : string
		window�ɁBAR���\�[�X���ǂ��z�u���邩�B
		chromakey�Aperspective�Aperspective + chromakey����I������B

	- ARUCO : bool
		BACKGROUND_TYPE��camera�̂Ƃ��AARUCO�}�[�J�[��Ƀ����_�����O���邩��I������B
		ARUCO�}�[�J�[��Ƀ����_�����O����̂ŁACOMBINE��perspective + chromakey�ɂ��邱�ƁB

	- ARUCO_VERTICAL : bool
		ARUCO��true�̂Ƃ��A���\�[�X���}�[�J�[��ɑ΂��Đ����ɔz�u���邩�A
		�����ɔz�u���邩��I������B


